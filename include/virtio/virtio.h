/*
 * Copyright (c) 2014 ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Universitaetsstrasse 6, CH-8092 Zurich. Attn: Systems Group.
 */

#ifndef VIRTIO_H
#define VIRTIO_H

#include <barrelfish/barrelfish.h>


/// defines how we map the memory frames
#define VIRTIO_VREGION_FLAGS_DEVICE VREGION_FLAGS_READ_WRITE
#define VIRTIO_VREGION_FLAGS_RING VREGION_FLAGS_READ_WRITE


/*
 * Generic Feature Bits
 */

/// Generate interrupt if queue is completely used
#define VIRTIO_F_NOTIFY_ON_EMPTY (1 << 24)

/// guest should never set this feature. This indicates faulty drivers
#define VIRTIO_F_BAD_FEATURE (1 << 30)

/// range of the transport related feature bits
#define VIRTIO_TRANSPORT_F_START    28
#define VIRTIO_TRANSPORT_F_END      32



/**
 * \brief initializes the library for host side operation
 *
 * \param guest_base    base address of the guest physical memory
 * \param guest_size    size of the guest physical memory
 */
errval_t virtio_host_init(lpaddr_t guest_base,
                          lpaddr_t guest_size);


/**
 * VirtIO Memory segment
 */

struct virtio_buffer_allocator;

enum virtio_buffer_state {
    VIRTIO_BUFFER_S_INVALID,
    VIRTIO_BUFFER_S_FREE,
    VIRTIO_BUFFER_S_ALLOCED,
    VIRTIO_BUFFER_S_QUEUED
};

/**
 * represents a VirtIO buffer to be used
 */
struct virtio_buffer
{
    struct virtio_buffer_allocator *a;
    enum virtio_buffer_state state;     ///< state of this buffer
    lpaddr_t paddr;                     ///< physical address of the buffer
    void *buf;                          ///< mapped virtual address of the buffer
    size_t   length;                    ///< size of this buffer
    struct virtio_buffer_list *lhead;   ///< pointer to the buffer list head
    struct virtio_buffer *next;         ///< pointer to the next buffer in the list
};

/**
 * represents a list of buffers
 */
struct virtio_buffer_list
{
    struct virtio_buffer *head;
    struct virtio_buffer *tail;
    size_t length;
};



/**
 * \brief   initializes the buffer allocator and allocates memory for the
 *          buffers
 *
 * \param   alloc   the allocator struct to initialize
 * \param   nbufs   number of buffers to allocate
 * \param   bufsz   size of each buffer to allocate
 *
 * \return  SYS_ERR_OK on success
 */
errval_t virtio_buffer_alloc_init(struct virtio_buffer_allocator **alloc,
                                  size_t nbufs,
                                  size_t bufsz);

/**
 * \brief   destroys a buffer allocator by freeing up all the resources used
 *          by the buffers
 *
 * \param   alloc   the allocator to destroy
 *
 * \returns SYS_ERR_OK on success
 */
errval_t virtio_buffer_alloc_destroy(struct virtio_buffer_allocator *alloc);

/**
 * \brief   allocated a new virti_buffer from the buffer allocator
 */
struct virtio_buffer *virtio_buffer_alloc(struct virtio_buffer_allocator *alloc);

/**
 * \brief   frees up a unused buffer by returning it to the allocator
 *
 * \param   buf     the buffer to be freed
 */
errval_t virtio_buffer_free(struct virtio_buffer *buf);

/**
 * \brief   returns the backing frame capability of a buffer allocator
 */
errval_t virtio_buffer_alloc_get_cap(struct virtio_buffer_allocator *alloc,
                                     struct capref *ret_cap);

/**
 * \brief initializes a new VirtIO buffer list to be used for chaining buffers
 *
 * \param bl buffer list to initialize
 *
 * \return SYS_ERR_OK on success
 */
errval_t virtio_blist_init(struct virtio_buffer_list *bl);

/**
 * \brief frees up the buffer list by returning the buffers to the allocator
 *
 * \param bl buffer list to be freed
 *
 * \returns SYS_ERR_OK on success
 */
errval_t virtio_blist_free(struct virtio_buffer_list *bl);

/**
 * \brief appends a buffer to the tail of buffer list
 *
 * \param bl    the list to append the buffer to
 * \param buf   the buffer to be appended
 */
errval_t virtio_blist_append(struct virtio_buffer_list *bl,
                             struct virtio_buffer *buf);

/**
 * \brief returns and removes the head of the list
 *
 * \param bl buffer list
 *
 * \returns pointer to virtio_buffer on sucess
 *          NULL on failuer
 */
struct virtio_buffer *virtio_blist_get(struct virtio_buffer_list *bl);



#endif // VIRTIO_H
