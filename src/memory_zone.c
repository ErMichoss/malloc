#include "../incl/malloc.h"
#include "../lib/printf/ft_printf.h"

void    show_alloc_mem(void)
{
    t_zone  *aux_zone;
    t_block *aux_block;
    void    *block_start;
    void    *block_end;
    size_t  total_bytes;

    total_bytes = 0;
    if (data.tiny_head != NULL)
    {
        ft_printf("TINY : %p\n", (void*)data.tiny_head);
        aux_zone = data.tiny_head;
        while (aux_zone != NULL)
        {
            aux_block = aux_zone->head;
            while (aux_block != NULL)
            {
                if (aux_block->is_free == false)
                {
                    block_start = (void *)((char *)aux_block + BLOCK_OFFSET);
                    block_end = (void *)((char *)aux_block + BLOCK_OFFSET + aux_block->size);
    
                    ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)aux_block->size);
                    total_bytes = total_bytes + aux_block->size;
                }
                aux_block = aux_block->next;
            }
            aux_zone = aux_zone->next;
        }
        ft_printf("Total : %u bytes\n", (unsigned int)total_bytes);
    }

    total_bytes = 0;
    if (data.small_head != NULL)
    {
        ft_printf("SMALL : %p\n", (void*)data.small_head);
        aux_zone = data.small_head;
        while (aux_zone != NULL)
        {
            aux_block = aux_zone->head;
            while (aux_block != NULL)
            {
                if (aux_block->is_free == false)
                {
                    block_start = (void *)((char *)aux_block + BLOCK_OFFSET);
                    block_end = (void *)((char *)aux_block + BLOCK_OFFSET + aux_block->size);
    
                    ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)aux_block->size);
                    total_bytes = total_bytes + aux_block->size;
                }
                aux_block = aux_block->next;
            }
            aux_zone = aux_zone->next;
        }
        ft_printf("Total : %u bytes\n", (unsigned int)total_bytes);
    }

    total_bytes = 0;
    if (data.large_head != NULL)
    {
        ft_printf("LARGE : %p\n", (void*)data.large_head);
        aux_zone = data.large_head;
        while (aux_zone != NULL)
        {
            aux_block = aux_zone->head;
            while (aux_block != NULL)
            {
                if (aux_block->is_free == false)
                {
                    block_start = (void *)((char *)aux_block + BLOCK_OFFSET);
                    block_end = (void *)((char *)aux_block + BLOCK_OFFSET + aux_block->size);
    
                    ft_printf("%p - %p : %u bytes\n", block_start, block_end, (unsigned int)aux_block->size);
                    total_bytes = total_bytes + aux_block->size;
                }
                aux_block = aux_block->next;
            }
            aux_zone = aux_zone->next;
        }
        ft_printf("Total : %u bytes\n", (unsigned int)total_bytes);
    }
}