#include "malloc.h"
#include "../lib/printf/ft_printf.h"

void    *block_get(size_t size, e_zone_type type) {
    t_zone  *zone;
    t_block *block;

    // Priority 1: Reuse a completely empty zone if one exists.
    zone = zone_recycle(type);
    if (zone) {
        block = block_find(zone->head, size);
        if (block) {
            block->is_free = false;
            block->type = type;
            if (debug_enabled())
                ft_printf("DEBUG: Malloc. Reusing empty zone at %p\n", zone);
            return ((void *)((char *)block + BLOCK_OFFSET));
        }
    }

    // Priority 2: Search for a free block in existing zones.
    if (type == TINY)
        zone = data.tiny_head;
    else
        zone = data.small_head;

    while (zone != NULL) {
        block = block_find(zone->head, size);
        if (block) {
            block->is_free = false;
            block->type = type;
            if (debug_enabled())
                ft_printf("DEBUG: Malloc in existing block. Zone address: %p\n", zone);
            return ((void *)((char *)block + BLOCK_OFFSET));
        }
        zone = zone->next;
    }

    // Priority 3: No empty zones or free blocks found, create a new zone.
    zone = zone_create(size, type);
    if (!zone)
        return (NULL);

    block = block_find(zone->head, size);
    if (block) {
        block->is_free = false;
        block->type = type;
        if (debug_enabled())
            ft_printf("DEBUG: Malloc. Creating new zone at %p\n", zone);
        return ((void *)((char *)block + BLOCK_OFFSET));
    }
    return (NULL);
}

void    block_split(t_block *block, size_t size) {
    t_block *remainder;
    size_t  original_size;

    original_size = block->size;
    if (original_size > size + BLOCK_OFFSET) {
        block->size = size;

        remainder           = (t_block *)((char *)block + size + BLOCK_OFFSET);
        remainder->size     = original_size - size - BLOCK_OFFSET;
        remainder->is_free  = true;
        remainder->prev     = block;
        remainder->next     = block->next;

        if (block->next != NULL)
            block->next->prev = remainder;
        block->next = remainder;

        if (remainder->next != NULL && remainder->next->is_free == true) {
            remainder->size = remainder->size + remainder->next->size + BLOCK_OFFSET;
            remainder->next = remainder->next->next;
            if (remainder->next != NULL)
                remainder->next->prev = remainder;
        }
    }
}

t_block *block_find(t_block *head, size_t size) {
    t_block *cursor;

    cursor = head;
    while (cursor != NULL) {
        if (cursor->is_free == true && cursor->size >= size) {
            block_split(cursor, size);
            return (cursor);
        }
        cursor = cursor->next;
    }
    return (NULL);
}