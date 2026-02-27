#include "malloc.h"
#include "../lib/printf/ft_printf.h"

void    *find_or_create_block(size_t size, e_zone_type type)
{
    t_zone  *zone;
    t_block *block;

    // Prioridad 1: Reutilizar una zona completamente vacía si existe.
    zone = reuse_empty_zone(type);
    if (zone)
    {
        block = find_and_split_block(zone->head, size);
        if (block)
        {
            block->is_free = false;
            block->type = type;
            if (is_debug_enabled())
                ft_printf("DEBUG: Malloc. Reusing empty area in %p\n", zone);
            return ((void *)((char *)block + BLOCK_OFFSET));
        }
    }

    // Prioridad 2: Buscar un bloque libre en las zonas ya existentes.
    if (type == TINY)
        zone = data.tiny_head;
    else
        zone = data.small_head;
    
    while (zone != NULL)
    {
        block = find_and_split_block(zone->head, size);
        if (block)
        {
            block->is_free = false;
            block->type = type;
            if (is_debug_enabled())
                ft_printf("DEBUG: Malloc in existing block. Zone address: %p\n", zone);
            return ((void *)(char *)block + BLOCK_OFFSET);
        }
        zone = zone->next;
    }

    // Prioridad 3: No hay zonas vacías ni bloques libres, crear una nueva zona.
    zone = create_new_zone(size, type);
    if (!zone)
        return (NULL);

    block = find_and_split_block(zone->head, size);
    if (block)
    {
        block->is_free = false;
        block->type = type;
        if (is_debug_enabled())
            ft_printf("DEBUG: Malloc. Creating new zone in %p\n", zone);
        return ((void *)(char *)block + BLOCK_OFFSET);
    }
    return (NULL);
}

void    split_block(t_block *block, size_t size)
{
    t_block *new_block;
    size_t  old_size;

    old_size = block->size;
    if (old_size > size + BLOCK_OFFSET)
    {
        block->size = size;

        new_block = (t_block *)((char *)block + size + BLOCK_OFFSET);
        new_block->size = old_size - size - BLOCK_OFFSET;
        new_block->is_free = true;

        new_block->prev = block;
        new_block->next = block->next;
        if (block->next != NULL)
            block->next->prev = new_block;
        block->next = new_block;

        if (new_block->next != NULL && new_block->next->is_free == true)
        {
            new_block->size = new_block->size + new_block->next->size + BLOCK_OFFSET;
            new_block->next = new_block->next->next;
            if (new_block->next != NULL)
                new_block->next->prev = new_block;
        }
    }
}

t_block *find_and_split_block(t_block *head, size_t size)
{
    t_block *aux;

    aux = head;
    while (aux != NULL)
    {
        if (aux->is_free == true && aux->size >= size)
        {
            split_block(aux, size);
            return(aux);
        }
        aux = aux->next;
    }
    return (NULL);
}
