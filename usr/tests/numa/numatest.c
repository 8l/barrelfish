/**
 * \file
 * \brief Test the LUA interpreter library
 */
/*
 * Copyright (c) 2013, ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

#include <barrelfish/barrelfish.h>

#include <numa.h>


int main (void)
{
    debug_printf("numa test started...\n");

    if (numa_available() == SYS_ERR_OK) {
        debug_printf("num nodes=%u\n", numa_max_node() + 1);
        debug_printf("num cores: %u\n", numa_max_core() + 1);
    } else {
        debug_printf("numa not available\n");
    }
    return 0;
}
