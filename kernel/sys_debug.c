/**
 * \file
 * \brief Arch-generic debug system calls implementation.
 */

/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012, ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <sys_debug.h>
#include <syscall.h>
#include <barrelfish_kpi/init.h>
#include <barrelfish_kpi/syscalls.h>
#include <capabilities.h>
#include <cap_predicates.h>
#include <coreboot.h>
#include <mdb/mdb.h>
#include <mdb/mdb_tree.h>
#include <cap_predicates.h>
#include <dispatch.h>
#include <distcaps.h>
#include <wakeup.h>
#include <paging_kernel_helper.h>
#include <paging_kernel_arch.h>
#include <exec.h>
#include <irq.h>
#include <trace/trace.h>
#include <trace_definitions/trace_defs.h>
#include <kcb.h>

static bool
sys_debug_print_capabilities_check_cnode(struct cte *cte, struct cte **dispatcher) {
    enum objtype type = cte->cap.type;

    if (type == ObjType_Dispatcher) {
        *dispatcher = cte;
        return true;
    }

    if (type == ObjType_KernelControlBlock) {
        // We are looking at init's rootcn, and found the reference to it in
        // the KCB.
        struct kcb *kcb = (struct kcb*) local_phys_to_mem(get_address(&cte->cap));
        struct cte *cn = (struct cte*) local_phys_to_mem(get_address(&kcb->init_rootcn.cap));
        struct cte *cn_task = (struct cte*) local_phys_to_mem(get_address(&cn[ROOTCN_SLOT_TASKCN].cap));
        *dispatcher = &cn_task[TASKCN_SLOT_DISPATCHER];
        return true;
    }

    struct cte *cn = (struct cte*) local_phys_to_mem(get_address(&cte->cap));

    if (type == ObjType_CNode) {
        // are we task cnode?
        if (cn[TASKCN_SLOT_DISPATCHER].cap.type == ObjType_Dispatcher) {
            *dispatcher = &cn[TASKCN_SLOT_DISPATCHER];
            return true;
        }

        if (cn[ROOTCN_SLOT_TASKCN].cap.type == ObjType_CNode) {
            struct cte *cn_task = (struct cte*) local_phys_to_mem(get_address(&cn[ROOTCN_SLOT_TASKCN].cap));

            if (cn_task[TASKCN_SLOT_DISPATCHER].cap.type == ObjType_Dispatcher) {
                *dispatcher = &cn_task[TASKCN_SLOT_DISPATCHER];
                return true;
            }
        }
    }

    return false;
}

static errval_t
sys_debug_print_capabilities_cb(struct cte *cte, void *data) {
//    printk(LOG_NOTE, "cte=%p\n", cte);

    struct dcb *my_dcb = (struct dcb *) data;

    struct cte *result;
    errval_t err = mdb_find_cap_for_address(mem_to_local_phys((lvaddr_t) cte), &result);
    if (err_is_fail(err)) {
        printk(LOG_ERR, "Type of cap: %d, kernel address %p, phys. address 0x%"PRIxLPADDR"\n", cte->cap.type, cte, mem_to_local_phys((lvaddr_t) cte));
        printk(LOG_ERR, "kcb_current = %p\n", kcb_current);
        printk(LOG_ERR, "%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
        mdb_dump_all_the_things();
        return err;
    }

    assert(result->cap.type == ObjType_CNode ||
           result->cap.type == ObjType_Dispatcher ||
           result->cap.type == ObjType_KernelControlBlock);

    struct cte *dispatcher;

    while (!sys_debug_print_capabilities_check_cnode(result, &dispatcher)) {
        err = mdb_find_cap_for_address(mem_to_local_phys((lvaddr_t) result), &result);
        if (err_is_fail(err)) {
            printk(LOG_ERR, "Type of cap: %d\n", cte->cap.type);
            printk(LOG_ERR, "%s:%s:%d \n", __FILE__, __FUNCTION__, __LINE__);
            return err;
        }
    }

    assert(dispatcher->cap.type == ObjType_Dispatcher);

    struct dcb *dcb = dispatcher->cap.u.dispatcher.dcb;
    dispatcher_handle_t handle = dcb->disp;
    struct dispatcher_shared_generic *disp =
        get_dispatcher_shared_generic(handle);

    if (my_dcb == dcb) {
        char buffer[256];
        sprint_cap(buffer, 256, &cte->cap);
        printk(LOG_NOTE, "disp->name=%s %s\n", disp->name, buffer);
    }

    return SYS_ERR_OK;
}

errval_t
debug_print_cababilities(struct dcb *dispatcher) {
    return mdb_traverse(MDB_TRAVERSAL_ORDER_ASCENDING, sys_debug_print_capabilities_cb, dispatcher);
}
