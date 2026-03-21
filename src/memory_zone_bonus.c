#include "malloc.h"
#include "../lib/printf/ft_printf.h"

void    show_alloc_mem_ex(void) {
    t_zone          *zone;
    t_block         *block;
    void            *block_start;
    void            *block_end;
    unsigned char   *data_ptr;
    size_t          total_bytes;
    size_t          byte_idx;
    int             col;
    int             count;

    total_bytes = 0;
    if (data.tiny_head != NULL) {
        ft_printf("TINY : %p\n", (void *)data.tiny_head);
        zone = data.tiny_head;
        while (zone != NULL) {
            block = zone->head;
            while (block != NULL) {
                block_start = (void *)((char *)block + BLOCK_OFFSET);
                block_end   = (void *)((char *)block + BLOCK_OFFSET + block->size);
                ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)block->size);
                if (block->is_free == false) {
                    data_ptr = (unsigned char *)((char *)block + BLOCK_OFFSET);
                    byte_idx = 0;
                    while (byte_idx < block->size) {
                        ft_printf("%p: ", &data_ptr[byte_idx]);
                        col = 0;
                        while (col < 16 && byte_idx < block->size) {
                            unsigned char byte = data_ptr[byte_idx];
                            count = 0;
                            if (byte < 16)
                                ft_putchar('0', &count);
                            ft_hexa(byte, "0123456789abcdef", &count);
                            ft_putchar(' ', &count);
                            byte_idx++;
                            col++;
                        }
                        ft_printf("\n");
                    }
                }
                total_bytes = total_bytes + block->size;
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
                block_start = (void *)((char *)block + BLOCK_OFFSET);
                block_end   = (void *)((char *)block + BLOCK_OFFSET + block->size);
                ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)block->size);
                if (block->is_free == false) {
                    data_ptr = (unsigned char *)((char *)block + BLOCK_OFFSET);
                    byte_idx = 0;
                    while (byte_idx < block->size) {
                        ft_printf("%p: ", &data_ptr[byte_idx]);
                        col = 0;
                        while (col < 16 && byte_idx < block->size) {
                            unsigned char byte = data_ptr[byte_idx];
                            count = 0;
                            if (byte < 16)
                                ft_putchar('0', &count);
                            ft_hexa(byte, "0123456789abcdef", &count);
                            ft_putchar(' ', &count);
                            byte_idx++;
                            col++;
                        }
                        ft_printf("\n");
                    }
                }
                total_bytes = total_bytes + block->size;
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
                block_start = (void *)((char *)block + BLOCK_OFFSET);
                block_end   = (void *)((char *)block + BLOCK_OFFSET + block->size);
                ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)block->size);
                if (block->is_free == false) {
                    data_ptr = (unsigned char *)((char *)block + BLOCK_OFFSET);
                    byte_idx = 0;
                    while (byte_idx < block->size) {
                        ft_printf("%p: ", &data_ptr[byte_idx]);
                        col = 0;
                        while (col < 16 && byte_idx < block->size) {
                            unsigned char byte = data_ptr[byte_idx];
                            count = 0;
                            if (byte < 16)
                                ft_putchar('0', &count);
                            ft_hexa(byte, "0123456789abcdef", &count);
                            ft_putchar(' ', &count);
                            byte_idx++;
                            col++;
                        }
                        ft_printf("\n");
                    }
                }
                total_bytes = total_bytes + block->size;
                block = block->next;
            }
            zone = zone->next;
        }
        ft_printf("Total : %u bytes\n", (unsigned int)total_bytes);
    }
}