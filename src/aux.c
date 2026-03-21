#include "malloc.h"
#include "../lib/printf/ft_printf.h"

void    str_print(char *str) {
    int i;

    i = 0;
    while (str[i] != '\0') {
        write(1, &str[i], 1);
        i++;
    }
    write(1, "\n", 1);
}

size_t  str_len(char *str) {
    int i;

    i = 0;
    while (str[i] != '\0')
        i++;
    return (i);
}

void    zone_append(t_zone **large_head, t_zone *zone) {
    t_zone  *cursor;

    cursor = *large_head;
    while (cursor->next)
        cursor = cursor->next;
    cursor->next = zone;
}

void    zone_append_small(t_zone **small_head, t_zone *zone) {
    t_zone  *cursor;

    cursor = *small_head;
    while (cursor->next)
        cursor = cursor->next;
    cursor->next = zone;
}

void    zone_append_tiny(t_zone **tiny_head, t_zone *zone) {
    t_zone  *cursor;

    cursor = *tiny_head;
    while (cursor->next)
        cursor = cursor->next;
    cursor->next = zone;
}

void    *mem_copy(void *dst, const void *src, size_t n) {
    char        *dst_ptr;
    const char  *src_ptr;
 
    dst_ptr = dst;
    src_ptr = src;
    while (n--)
        *dst_ptr++ = *src_ptr++;
    return (dst);
}