#include "../incl/malloc.h"
#include "../lib/printf/ft_printf.h"

void    *ft_memcpy(void *dst, const void *src, size_t n)
{
    char *d = dst;
    const char *s = src;
    while (n--)
        *d++ = *s++;
    return (dst);
}
