#include "../incl/malloc.h"
#include "../lib/printf/ft_printf.h"

void    *realloc(void *ptr, size_t size)
{
    t_zone *zone;
    t_block *block;
    t_block *aux_block;
    size_t  aligned_size;
    void    *new_ptr;
    
    if (!ptr)   
        return (malloc(size));
    
    if (size == 0)
    {
        free(ptr);
        return (NULL);
    }
    
    block = (t_block *)((char *)ptr - BLOCK_OFFSET);
    zone = find_zone_for_ptr(ptr);
    if (!zone)
    {
        ft_printf("*** Error: invalid pointer ***\n");
        return (NULL);
    }

    aligned_size = (size + sizeof(void *) -1) & ~(sizeof(void *) - 1);
    if (is_debug_enabled())
        ft_printf("DEBUG: realloc para el block: %p. Tamaño solicitado: %u. Tamaño alineado: %u\n", ptr, size, (unsigned int)aligned_size);

    if (aligned_size <= block->size)
    {
        if (block->size >= aligned_size + BLOCK_OFFSET)
            split_block(block, aligned_size);
        return (ptr);
    }

    if (block->next != NULL && block->next->is_free == true &&
        (block->size + block->next->size + BLOCK_OFFSET) >= aligned_size) 
    {
        aux_block = block->next;
        block->size = block->size + aux_block->size + BLOCK_OFFSET;
        block->next = aux_block->next;
        if (block->next != NULL)
            block->next->prev = block;
        
        if (block->size - aligned_size > BLOCK_OFFSET)
            split_block(block, aligned_size);
        if (is_debug_enabled())
            ft_printf("DEBUG: Realloc TINY/SMALL. Ampliando in-place. Nuevo tamaño del block: %u\n", (unsigned int)block->size);
        return (ptr);
    }

    new_ptr = malloc(size);
    if (!new_ptr)
        return (NULL);

    ft_memcpy(new_ptr, ptr, block->size);
    free(ptr);
    
    if (is_debug_enabled())
        ft_printf("DEBUG: Realloc TINY/SMALL. Nueva Asignación. Nueva dirección: %p. Nuevo tamaño: %u\n", new_ptr, (unsigned int)size);
    return(new_ptr);
}