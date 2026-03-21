#ifndef MALLOC_H
# define MALLOC_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <stdbool.h>
# include <sys/mman.h>
# include <errno.h>
# include <pthread.h>

# define BLOCK_OFFSET       32

# define TINY_MAX_SIZE      128
# define SMALL_MAX_SIZE     1024

# define BLOCKS_PER_ZONE    100

typedef enum {
    TINY,
    SMALL,
    LARGE
}   e_zone_type;

typedef struct s_block {
    size_t          size;
    bool            is_free;
    e_zone_type     type;
    struct s_block  *next;
    struct s_block  *prev;
}   t_block;

typedef struct s_zone {
    t_block         *head;
    size_t          total_size;
    struct s_zone   *next;
    char            padding[8];
}   t_zone;

typedef struct s_malloc_data {
    t_zone  *tiny_head;
    t_zone  *small_head;
    t_zone  *large_head;
    t_zone  *empty_tiny_head;
    t_zone  *empty_small_head;
}   t_malloc_data;

extern t_malloc_data    data;

/* ___ AUX ___ */

/**
 * @brief Prints a string to stdout followed by a newline.
 * @param str The string to print.
 * @return void
 */
void    str_print(char *str);

/**
 * @brief Returns the length of a null-terminated string.
 * @param str The string to measure.
 * @return The number of characters before the null terminator.
 */
size_t  str_len(char *str);

/**
 * @brief Appends a zone to the end of the large zone list.
 * @param large_head Pointer to the head of the large zone list.
 * @param zone The zone to append.
 * @return void
 */
void    zone_append(t_zone **large_head, t_zone *zone);

/**
 * @brief Appends a zone to the end of the small zone list.
 * @param small_head Pointer to the head of the small zone list.
 * @param zone The zone to append.
 * @return void
 */
void    zone_append_small(t_zone **small_head, t_zone *zone);

/**
 * @brief Appends a zone to the end of the tiny zone list.
 * @param tiny_head Pointer to the head of the tiny zone list.
 * @param zone The zone to append.
 * @return void
 */
void    zone_append_tiny(t_zone **tiny_head, t_zone *zone);

/* ___ AUX_TWO ___ */

/**
 * @brief Copies n bytes from src to dst.
 * @param dest Destination buffer.
 * @param src Source buffer.
 * @param n Number of bytes to copy.
 * @return Pointer to the destination buffer.
 */
void    *mem_copy(void *dest, const void *src, size_t n);

/* ___ BLOCKS ___ */

/**
 * @brief Finds a free block in existing zones or creates a new zone to allocate one.
 * @param size The requested allocation size in bytes.
 * @param type The zone type (TINY or SMALL).
 * @return Pointer to the usable memory area, or NULL on failure.
 */
void    *block_get(size_t size, e_zone_type type);

/**
 * @brief Splits a block into two if it is large enough to hold the remainder.
 * @param block The block to split.
 * @param size The size to keep in the original block.
 * @return void
 */
void    block_split(t_block *block, size_t size);

/**
 * @brief Searches a block list for a free block of sufficient size and splits it.
 * @param head The head of the block list to search.
 * @param size The minimum required size.
 * @return Pointer to the found block, or NULL if none found.
 */
t_block *block_find(t_block *head, size_t size);

/* ___ FREE ___ */

/**
 * @brief Finds the zone that contains the given pointer.
 * @param ptr The pointer to locate.
 * @return Pointer to the owning zone, or NULL if not found.
 */
t_zone  *zone_of(void *ptr);

/**
 * @brief Frees a previously allocated memory block.
 * @param ptr Pointer to the memory to free.
 * @return void
 */
void    free(void *ptr);

/* ___ MALLOC ___ */

/**
 * @brief Rounds a size up to the nearest multiple of the system page size.
 * @param total_size The size to round up.
 * @return The rounded-up size aligned to a page boundary.
 */
size_t  align_to_page(size_t total_size);

/**
 * @brief Attempts to reuse a previously emptied zone of the given type.
 * @param type The zone type to look for (TINY or SMALL).
 * @return Pointer to the recycled zone, or NULL if none available.
 */
t_zone  *zone_recycle(e_zone_type type);

/**
 * @brief Creates and maps a new zone for the given allocation size and type.
 * @param size The requested allocation size in bytes.
 * @param type The zone type (TINY or SMALL).
 * @return Pointer to the new zone, or NULL on failure.
 */
void    *zone_create(size_t size, e_zone_type type);

/**
 * @brief Allocates size bytes of memory.
 * @param size Number of bytes to allocate.
 * @return Pointer to the allocated memory, or NULL on failure.
 */
void    *malloc(size_t size);

/* ___ REALLOC ___ */

/**
 * @brief Resizes a previously allocated memory block.
 * @param ptr Pointer to the existing allocation, or NULL to behave like malloc.
 * @param size The new requested size in bytes.
 * @return Pointer to the resized memory, or NULL on failure.
 */
void    *realloc(void *ptr, size_t size);

/* ___ MEMORY_ZONE ___ */

/**
 * @brief Prints a summary of all currently allocated memory blocks.
 * @return void
 */
void    show_alloc_mem(void);

/* ___ MEMORY_ZONE_BONUS ___ */

/**
 * @brief Prints a detailed hex dump of all allocated memory blocks.
 * @return void
 */
void    show_alloc_mem_ex(void);

/* ___ GLOBALS ___ */

/**
 * @brief Returns whether the MALLOC_VERBOSE environment variable is set.
 * @return 1 if debug is enabled, 0 otherwise.
 */
int     debug_enabled(void);

#endif