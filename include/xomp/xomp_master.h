/*
 * Copyright (c) 2014 ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Universitaetsstrasse 6, CH-8092 Zurich. Attn: Systems Group.
 */

#ifndef LIB_XOMP_MASTER_H_
#define LIB_XOMP_MASTER_H_

/**
 * \brief initializes the Xeon Phi openMP library
 *
 * \param nphi  Number of Xeon Phis to use
 * \param path  Path to the worker binary
 * \param argc  Argument count this program was started
 * \param argv  Argumnent values
 *
 * \returns SYS_ERR_OK on success
 *          errval on failure
 */
errval_t xomp_master_init(uint8_t nphi,
                          char *path,
                          uint8_t argc,
                          char *argv[]);

/**
 * \brief Spawns the worker threads on the Xeon Phi
 *
 * \param nworkers  Number of worker thread to be spawned per Xeon Phi
 *
 * \returns SYS_ERR_OK on success
 *          errval on failure
 */
errval_t xomp_master_spawn_workers(uint32_t nworkers);

/**
 * \brief Adds a memory region to be used for work
 *
 * \param frame Frame to be shared
 * \param vbase virtual base address where the frame should be mapped
 *
 * \returns SYS_ERR_OK on success
 *          errval on error
 */
errval_t xomp_master_add_memory(struct capref frame, lvaddr_t vbase);

/**
 * \brief executes some work on each worker domains
 *
 * \param fn
 * \param arg
 * \param nthreads
 *
 * \returns SYS_ERR_OK on success
 *          errval on error
 */
errval_t xomp_master_do_work(lvaddr_t fn, lvaddr_t arg, uint32_t nthreads);

#endif // LIB_XOMP_MASTER_H_