#include "malloc.h"
#include "../lib/printf/ft_printf.h"

void    *realloc(void *ptr, size_t size) {
    t_zone  *zone;
    t_block *block;
    t_block *neighbor;
    size_t  aligned_size;
    void    *new_ptr;

    if (!ptr)
        return (malloc(size));

    if (size == 0) {
        free(ptr);
        return (NULL);
    }

    block = (t_block *)((char *)ptr - BLOCK_OFFSET);
    zone  = zone_of(ptr);
    if (!zone) {
        ft_printf("*** Error: invalid pointer ***\n");
        return (NULL);
    }

    aligned_size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
    if (debug_enabled())
        ft_printf("DEBUG: realloc for block: %p. Requested: %u. Aligned: %u\n",
            ptr, (unsigned int)size, (unsigned int)aligned_size);

    if (aligned_size <= block->size) {
        if (block->size >= aligned_size + BLOCK_OFFSET)
            block_split(block, aligned_size);
        return (ptr);
    }

    if (block->next != NULL && block->next->is_free == true &&
        (block->size + block->next->size + BLOCK_OFFSET) >= aligned_size) {
        neighbor    = block->next;
        block->size = block->size + neighbor->size + BLOCK_OFFSET;
        block->next = neighbor->next;
        if (block->next != NULL)
            block->next->prev = block;

        if (block->size - aligned_size > BLOCK_OFFSET)
            block_split(block, aligned_size);
        if (debug_enabled())
            ft_printf("DEBUG: Realloc TINY/SMALL. Expanded in-place. New block size: %u\n",
                (unsigned int)block->size);
        return (ptr);
    }

    new_ptr = malloc(aligned_size);
    if (!new_ptr)
        return (NULL);

    mem_copy(new_ptr, ptr, block->size);
    free(ptr);

    if (debug_enabled())
        ft_printf("DEBUG: Realloc TINY/SMALL. New allocation. Address: %p. Size: %u\n",
            new_ptr, (unsigned int)aligned_size);
    return (new_ptr);
}