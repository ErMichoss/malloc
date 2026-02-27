#include "malloc.h"
#include "../lib/printf/ft_printf.h"

void    print_str(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        write(1, &str[i], 1);
        i++;
    }
    write (1, "\n", 1);
}

size_t  ft_strlen(char *str)
{
    int i;

    i = 0;
    while (str[i] != '\0')
        i++;
    return (i);
}

void    append_zone(t_zone **large_head, t_zone *zone)
{
    t_zone  *aux;
    aux = *large_head;
    while (aux->next)
        aux = aux->next;
     aux->next = zone;
}

void    append_small_zone(t_zone **small_head, t_zone *zone)
{
    t_zone  *aux;
    aux = *small_head;
    while (aux->next)
        aux = aux->next;
     aux->next = zone;
}

void    append_tiny_zone(t_zone **tiny_head, t_zone *zone)
{
    t_zone  *aux;
    aux = *tiny_head;
    while (aux->next)
        aux = aux->next;
     aux->next = zone;
}