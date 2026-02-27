#include "malloc.h"
#include "../lib/printf/ft_printf.h"

t_malloc_data   data = {NULL, NULL, NULL, NULL, NULL};

static int  debug_enabled = -1;

int is_debug_enabled(void)
{
    char *env;

    if (debug_enabled == -1)
    {
        env = getenv("MALLOC_VERBOSE");
        debug_enabled = (env != NULL);
    }
    return (debug_enabled);
}