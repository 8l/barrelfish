/*
 * Copyright (c) 2012 ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

#ifndef CAPOPS_H
#define CAPOPS_H

#include <barrelfish/caddr.h>
#include <barrelfish/types.h>
#include <barrelfish_kpi/capabilities.h>
#include "domcap.h"

typedef void (*copy_result_handler_t)(errval_t, capaddr_t, uint8_t, void*);
errval_t copy(struct capref src, coreid_t dest,
              copy_result_handler_t result_handler, void *st);

typedef void (*move_result_handler_t)(errval_t, void*);
errval_t move(struct domcapref cap, coreid_t dest,
              move_result_handler_t result_handler, void *st);

typedef void (*delete_result_handler_t)(errval_t, void*);
errval_t delete(struct domcapref cap,
                delete_result_handler_t result_handler, void *st);

typedef void (*revoke_result_handler_t)(errval_t, void*);
errval_t revoke(struct domcapref cap,
                revoke_result_handler_t result_handler, void *st);

typedef void (*retype_result_handler_t)(errval_t, void*);
errval_t retype(enum objtype type, size_t objbits, struct capref croot,
                capaddr_t dest_cn, uint8_t dest_bits, cslot_t dest_slot,
                capaddr_t src, uint8_t src_bits, retype_result_handler_t
                result_handler, void *st);

#endif
