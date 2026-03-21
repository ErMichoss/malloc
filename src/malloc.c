#define _GNU_SOURCE
#include "malloc.h"
#include "../lib/printf/ft_printf.h"

size_t  align_to_page(size_t total_size) {
    size_t  page_size;
    size_t  num_pages;

    page_size = getpagesize();
    num_pages = (total_size + page_size - 1) / page_size;
    return (num_pages * page_size);
}

t_zone  *zone_recycle(e_zone_type type) {
    t_zone  **head_empty;
    t_zone  **head_current;
    t_zone  *zone;

    if (type == TINY) {
        head_empty   = &data.empty_tiny_head;
        head_current = &data.tiny_head;
    } else {
        head_empty   = &data.empty_small_head;
        head_current = &data.small_head;
    }

    if (*head_empty) {
        zone          = *head_empty;
        *head_empty   = zone->next;
        zone->next    = *head_current;
        *head_current = zone;
        if (debug_enabled())
            ft_printf("DEBUG: Recycling empty zone at %p\n", zone);
        return (zone);
    }
    return (NULL);
}

void    *zone_create(size_t len, e_zone_type type) {
    t_zone          *new_zone;
    t_block         *block;
    size_t          aligned_size;
    size_t          total_size;
    unsigned char   *ptr;

    total_size   = ((len + BLOCK_OFFSET) * BLOCKS_PER_ZONE);
    aligned_size = align_to_page(total_size);

    ptr = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED) {
        str_print("Error: malloc failed to allocate [SIZE] bytes");
        return (NULL);
    }

    new_zone = (t_zone *)ptr;
    block    = (t_block *)((char *)ptr + sizeof(t_zone));

    new_zone->head       = block;
    new_zone->next       = NULL;
    new_zone->total_size = aligned_size;

    block->size    = aligned_size - sizeof(t_zone) - BLOCK_OFFSET;
    block->is_free = true;
    block->next    = NULL;
    block->prev    = NULL;
    block->type    = type;

    if (type == TINY) {
        if (!data.tiny_head)
            data.tiny_head = new_zone;
        else
            zone_append_tiny(&data.tiny_head, new_zone);
    } else {
        if (!data.small_head)
            data.small_head = new_zone;
        else
            zone_append_small(&data.small_head, new_zone);
    }
    return ((void *)(new_zone));
}

void    *malloc(size_t size) {
    t_zone          *zone;
    t_block         *block;
    size_t          aligned_size;
    size_t          total_size;
    unsigned char   *ptr;

    if (size == 0)
        return (NULL);

    if (size <= TINY_MAX_SIZE)
        return (block_get(size, TINY));
    else if (size <= SMALL_MAX_SIZE)
        return (block_get(size, SMALL));
    else {
        total_size   = size + BLOCK_OFFSET + sizeof(t_zone);
        aligned_size = align_to_page(total_size);

        ptr = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (ptr == MAP_FAILED) {
            str_print("Error: malloc failed to allocate [SIZE] bytes");
            return (NULL);
        }

        zone  = (t_zone *)ptr;
        block = (t_block *)((char *)ptr + sizeof(t_zone));

        zone->head       = block;
        zone->total_size = aligned_size;
        zone->next       = NULL;

        block->is_free = false;
        block->size    = size;
        block->type    = LARGE;
        block->next    = NULL;
        block->prev    = NULL;

        if (!data.large_head)
            data.large_head = zone;
        else
            zone_append(&data.large_head, zone);

        if (debug_enabled())
            ft_printf("DEBUG: Malloc on LARGE block. Zone: %p, Block: %p, Size: %u\n",
                zone, (void *)((char *)block + BLOCK_OFFSET), (unsigned int)block->size);
        return ((void *)((char *)block + BLOCK_OFFSET));
    }
    return (NULL);
}