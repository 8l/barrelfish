/*
 * Copyright (c) 2012, ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

#include "caplock.h"
#include <barrelfish/waitset.h>
#include <barrelfish/event_queue.h>
#include <barrelfish/debug.h>
#include <monitor_invocations.h>
#include "capqueue.h"
#include "magic.h"

static struct capqueue_queue global_queue;

void
caplock_wait(struct domcapref cap,
             struct event_queue_node *qn, struct event_closure cont)
{
    debug_printf("caplock_wait\n");
    capqueue_wait(&global_queue, qn, cont);
}

void
caplock_unlock(struct domcapref cap)
{
    errval_t err = monitor_unlock_cap(cap.croot,
                                      cap.cptr >> (CPTR_BITS-cap.bits),
                                      cap.bits);
    if (err_is_fail(err)) {
        USER_PANIC_ERR(err, "unlocking cap");
    }
    capqueue_notify(&global_queue);
}

void
caplock_init(struct waitset *ws)
{
    capqueue_init(&global_queue, ws);
}
