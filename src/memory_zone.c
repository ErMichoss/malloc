#include "malloc.h"
#include "../lib/printf/ft_printf.h"

void    show_alloc_mem(void) {
    t_zone  *zone;
    t_block *block;
    void    *block_start;
    void    *block_end;
    size_t  total_bytes;

    total_bytes = 0;
    if (data.tiny_head != NULL) {
        ft_printf("TINY : %p\n", (void *)data.tiny_head);
        zone = data.tiny_head;
        while (zone != NULL) {
            block = zone->head;
            while (block != NULL) {
                if (block->is_free == false) {
                    block_start = (void *)((char *)block + BLOCK_OFFSET);
                    block_end   = (void *)((char *)block + BLOCK_OFFSET + block->size);
                    ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)block->size);
                    total_bytes = total_bytes + block->size;
                }
                block = block->next;
            }
            zone = zone->next;
        }
        ft_printf("Total : %u bytes\n", (unsigned int)total_bytes);
    }

    total_bytes = 0;
    if (data.small_head != NULL) {
        ft_printf("SMALL : %p\n", (void *)data.small_head);
        zone = data.small_head;
        while (zone != NULL) {
            block = zone->head;
            while (block != NULL) {
                if (block->is_free == false) {
                    block_start = (void *)((char *)block + BLOCK_OFFSET);
                    block_end   = (void *)((char *)block + BLOCK_OFFSET + block->size);
                    ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)block->size);
                    total_bytes = total_bytes + block->size;
                }
                block = block->next;
            }
            zone = zone->next;
        }
        ft_printf("Total : %u bytes\n", (unsigned int)total_bytes);
    }

    total_bytes = 0;
    if (data.large_head != NULL) {
        ft_printf("LARGE : %p\n", (void *)data.large_head);
        zone = data.large_head;
        while (zone != NULL) {
            block = zone->head;
            while (block != NULL) {
                if (block->is_free == false) {
                    block_start = (void *)((char *)block + BLOCK_OFFSET);
                    block_end   = (void *)((char *)block + BLOCK_OFFSET + block->size);
                    ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)block->size);
                    total_bytes = total_bytes + block->size;
                }
                block = block->next;
            }
            zone = zone->next;
        }
        ft_printf("Total : %u bytes\n", (unsigned int)total_bytes);
    }
}