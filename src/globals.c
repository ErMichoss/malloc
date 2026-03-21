#include "malloc.h"
#include "../lib/printf/ft_printf.h"

t_malloc_data   data = {NULL, NULL, NULL, NULL, NULL};

static int  debug_cache = -1;

int debug_enabled(void) {
    char *env;

    if (debug_cache == -1) {
        env         = getenv("MALLOC_VERBOSE");
        debug_cache = (env != NULL);
    }
    return (debug_cache);
}