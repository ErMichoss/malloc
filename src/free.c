#include "malloc.h"
#include "../lib/printf/ft_printf.h"

t_zone  *zone_of(void *ptr) {
    t_zone  *cursor;

    cursor = data.tiny_head;
    while (cursor) {
        if ((void *)cursor <= ptr && ptr < (void *)((char *)cursor + cursor->total_size))
            return (cursor);
        cursor = cursor->next;
    }

    cursor = data.small_head;
    while (cursor) {
        if ((void *)cursor <= ptr && ptr < (void *)((char *)cursor + cursor->total_size))
            return (cursor);
        cursor = cursor->next;
    }

    cursor = data.large_head;
    while (cursor) {
        if ((void *)cursor <= ptr && ptr < (void *)((char *)cursor + cursor->total_size))
            return (cursor);
        cursor = cursor->next;
    }

    return (NULL);
}

static void zone_unlink(t_zone *target) {
    t_zone  *prev;
    t_zone  *cursor;

    prev = NULL;
    if (target->head->type == TINY)
        cursor = data.tiny_head;
    else
        cursor = data.small_head;

    while (cursor) {
        if (cursor == target) {
            if (prev)
                prev->next = cursor->next;
            else {
                if (target->head->type == TINY)
                    data.tiny_head = cursor->next;
                else
                    data.small_head = cursor->next;
            }
            return;
        }
        prev = cursor;
        cursor = cursor->next;
    }
}

static void zone_unlink_large(t_zone *target) {
    t_zone  *prev;
    t_zone  *cursor;

    prev   = NULL;
    cursor = data.large_head;

    while (cursor) {
        if (cursor == target) {
            if (prev)
                prev->next = cursor->next;
            else
                data.large_head = cursor->next;
            return;
        }
        prev = cursor;
        cursor = cursor->next;
    }
}

void    free(void *ptr) {
    t_block *block;
    t_block *neighbor;
    t_block *cursor;
    t_zone  *zone;

    if (!ptr)
        return;

    // 1. Find the memory block and the zone it belongs to.
    zone = zone_of(ptr);
    if (!zone) {
        str_print("*** Error: double free detected or invalid pointer ***");
        if (debug_enabled())
            ft_printf("DEBUG: Error: double free or invalid pointer: %p\n", ptr);
        exit(1);
    }

    block = (t_block *)((char *)ptr - BLOCK_OFFSET);
    if (block->is_free == true) {
        str_print("*** Error: double free detected ***");
        if (debug_enabled())
            ft_printf("DEBUG: Error: double free detected: %p\n", ptr);
        exit(1);
    }

    if (block->type == LARGE) {
        // 2. Unlink the zone from the list and release it (LARGE).
        zone_unlink_large(zone);
        if (munmap(zone, zone->total_size) == -1) {
            str_print("Error: munmap failed for address");
            return;
        }
        if (debug_enabled())
            ft_printf("DEBUG: Free on LARGE block. Released address: %p\n", ptr);
        return;
    }
    else {
        block->is_free = true;
        if (debug_enabled())
            ft_printf("DEBUG: Free on TINY/SMALL block. Released address: %p\n", ptr);

        // 3. Merge adjacent free blocks (Coalescing).
        if (block->next != NULL && block->next->is_free == true) {
            neighbor     = block->next;
            block->size  = block->size + neighbor->size + BLOCK_OFFSET;
            block->next  = neighbor->next;
            if (block->next != NULL)
                block->next->prev = block;
        }
        if (block->prev != NULL && block->prev->is_free == true) {
            neighbor         = block->prev;
            neighbor->size   = neighbor->size + block->size + BLOCK_OFFSET;
            neighbor->next   = block->next;
            if (neighbor->next != NULL)
                neighbor->next->prev = neighbor;
            block = neighbor;
        }

        // Check if the entire zone is now free.
        cursor = zone->head;
        while (cursor != NULL) {
            if (cursor->is_free == false)
                return;
            cursor = cursor->next;
        }

        // Move the zone to the empty zones list.
        zone_unlink(zone);
        if (zone->head->type == TINY) {
            zone->next           = data.empty_tiny_head;
            data.empty_tiny_head = zone;
        } else {
            zone->next            = data.empty_small_head;
            data.empty_small_head = zone;
        }
        if (debug_enabled())
            ft_printf("DEBUG: Completely free zone, moving to the list of empty zones.\n");
    }
}