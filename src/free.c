#include "../incl/malloc.h"
#include "../lib/printf/ft_printf.h"

t_zone  *find_zone_for_ptr(void *ptr)
{
    t_zone  *current;

    current = data.tiny_head;
    while (current)
    {
        if ((void *)current <= ptr && ptr < (void *)((char *)current + current->total_size))
            return current;
        current = current->next;
    }

    current = data.small_head;
    while (current)
    {
        if ((void *)current <= ptr && ptr < (void*)((char *)current + current->total_size))
            return current;
        current = current->next;
    }

    current = data.large_head;
    while (current)
    {
        if ((void *)current <= ptr && ptr < (void*)((char *)current + current->total_size))
            return current;
        current = current->next;
    }
    
    return (NULL);
    
}

void    remove_zone_list(t_zone *zone_to_remove)
{
    t_zone  *prev;
    t_zone  *current;

    prev = NULL;
    if (zone_to_remove->head->type == TINY)
        current = data.tiny_head;
    else
        current = data.small_head;
    while (current)
    {
        if (current == zone_to_remove)
        {
            if (prev)
                prev->next = current->next;
            else
            {
                if (zone_to_remove->head->type == TINY)
                    data.tiny_head = current->next;
                else
                    data.small_head = current->next;
            }
            return;
        }
        prev = current;
        current = current->next;
    }
}
void    remove_large_zone(t_zone *zone_to_remove)
{
    t_zone  *prev;
    t_zone  *current;

    prev = NULL;
    current = data.large_head;

    while (current)
    {
        if (current == zone_to_remove)
        {
            if (prev)
                prev->next = current->next;
            else
                data.large_head = current->next;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void    free(void *ptr)
{
    t_block *data_block;
    t_block *aux_block;
    t_block *head_block;
    t_zone  *zone;

    if (!ptr)
        return;
    
    // 1. Encontrar el bloque de memoria y la zona a la que pertenece.
    zone = find_zone_for_ptr(ptr);
    if (!zone)
    {
        print_str("*** Error: double free detected or invalid pointer ***");
        if (is_debug_enabled())
            ft_printf("DEBUG: Error: double free detected or invalid pointer: %p\n", ptr);
        exit(1);
    }

    data_block = (t_block *)((char *)ptr - BLOCK_OFFSET);
    if (data_block->is_free == true)
    {
        print_str("*** Error: double free detected ***\n");
        if (is_debug_enabled())
            ft_printf("DEBUG: Error: double free detected: %p\n", ptr);
        exit(1);
    }

    if (data_block->type == LARGE)
    {
        // 2. Desenlazar la zona de la lista y liberar (LARGE).
        remove_large_zone(zone);
        if (munmap(zone, zone->total_size) == -1)
        {
            print_str("Error: munmap failed for address");  
            return;
        }
        if (is_debug_enabled())
            ft_printf("DEBUG Free en block LARGE. Dirección liberada: %p\n", ptr);
        return;
    }
    else
    {
        data_block->is_free = true;
        if (is_debug_enabled())
            ft_printf("DEBUG: Free en block TINY/SMALL. Dirección liberada: %p\n", ptr);
        
        // 3. Fusión de bloques adyacentes libres (Coalescencia)
        if (data_block->next != NULL && data_block->next->is_free == true)
        {
            aux_block = data_block->next;
            data_block->size = data_block->size + aux_block->size + BLOCK_OFFSET;
            data_block->next = aux_block->next;
            if (data_block->next != NULL)
                data_block->next->prev = data_block;
        }
        if (data_block->prev != NULL && data_block->prev->is_free == true)
        {
            aux_block = data_block->prev;
            aux_block->size = aux_block->size + data_block->size + BLOCK_OFFSET;
            aux_block->next = data_block->next;
            if (aux_block->next != NULL)
                aux_block->next->prev = aux_block;
            
            data_block = aux_block;
        }

        head_block = zone->head;
        while (head_block != NULL)
        {
            if (head_block->is_free == false)
                return;
            head_block = head_block->next;
        }
        
        // Si llegamos aquí, toda la zona está libre.
        remove_zone_list(zone);

        // Y la movemos a la nueva lista de zonas vacías
        if (zone->head->type == TINY)
        {
            zone->next = data.empty_tiny_head;
            data.empty_tiny_head = zone;
        }
        else
        {
            zone->next = data.empty_small_head;
            data.empty_small_head = zone;
        }
        if (is_debug_enabled())
            ft_printf("DEBUG: Completely free zone, moving to the list of empty zones.\n");
    }
}
