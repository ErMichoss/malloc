# Arquitectura y estructuras de datos

## Visión general

El allocator organiza la memoria en una jerarquía de dos niveles: **zonas** y **bloques**.

```
t_malloc_data (global)
├── tiny_head  ──► t_zone ──► t_zone ──► NULL
├── small_head ──► t_zone ──► NULL
├── large_head ──► t_zone ──► t_zone ──► NULL
├── empty_tiny_head  ──► t_zone ──► NULL   (zonas reciclables)
└── empty_small_head ──► t_zone ──► NULL   (zonas reciclables)
```

Cada `t_zone` contiene una lista doblemente enlazada de `t_block`.

---

## Estructuras

### `t_block` — Cabecera de bloque (32 bytes)

```c
typedef struct s_block {
    size_t          size;      // Tamaño del área de datos (sin incluir la cabecera)
    bool            is_free;   // Estado del bloque
    e_zone_type     type;      // TINY | SMALL | LARGE
    struct s_block  *next;     // Siguiente bloque en la zona
    struct s_block  *prev;     // Bloque anterior (para coalescencia)
} t_block;
```

`BLOCK_OFFSET = 32` es el tamaño fijo de esta cabecera. El puntero que devuelve `malloc` apunta **justo después** de ella:

```
┌──────────────────────────┬──────────────────────────────────┐
│  t_block (32 bytes)      │  área de datos (size bytes)      │
│  size | is_free | type   │  ◄── puntero devuelto al usuario │
│  next | prev             │                                  │
└──────────────────────────┴──────────────────────────────────┘
         ▲                          ▲
         ptr - BLOCK_OFFSET         ptr (lo que recibe el usuario)
```

### `t_zone` — Región de memoria mapeada

```c
typedef struct s_zone {
    t_block       *head;        // Primer bloque de la zona
    size_t         total_size;  // Tamaño total mapeado (múltiplo de página)
    struct s_zone *next;        // Siguiente zona de la misma categoría
    char           padding[8];  // Alineamiento
} t_zone;
```

La zona y sus bloques comparten la misma región de `mmap`. El layout en memoria es:

```
┌─────────────┬──────────────────────────────────────────────┐
│  t_zone     │  t_block[0] │ datos │ t_block[1] │ datos │…  │
│  (header)   │  (32 bytes) │       │  (32 bytes) │       │   │
└─────────────┴──────────────────────────────────────────────┘
▲ base del mmap
```

### `t_malloc_data` — Estado global

```c
typedef struct s_malloc_data {
    t_zone *tiny_head;        // Lista de zonas TINY activas
    t_zone *small_head;       // Lista de zonas SMALL activas
    t_zone *large_head;       // Lista de zonas LARGE (1 bloque por zona)
    t_zone *empty_tiny_head;  // Zonas TINY completamente libres (reciclaje)
    t_zone *empty_small_head; // Zonas SMALL completamente libres (reciclaje)
} t_malloc_data;

extern t_malloc_data data;  // Definida en globals.c
```

---

## Constantes clave

| Constante | Valor | Significado |
|-----------|-------|-------------|
| `BLOCK_OFFSET` | 32 | Tamaño de la cabecera `t_block` |
| `TINY_MAX_SIZE` | 128 | Umbral superior para bloques TINY |
| `SMALL_MAX_SIZE` | 1024 | Umbral superior para bloques SMALL |
| `BLOCKS_PER_ZONE` | 100 | Capacidad objetivo por zona |

### Cálculo del tamaño de zona

Al crear una zona nueva, se reserva espacio para `BLOCKS_PER_ZONE` bloques del tamaño máximo de la categoría y se redondea al múltiplo de `getpagesize()` más cercano:

```c
total_size   = (len + BLOCK_OFFSET) * BLOCKS_PER_ZONE;
aligned_size = round_up_to_page_size(total_size);
```

---

## Categorías de asignación

```
size ≤ 128       →  TINY   →  busca/crea zona tiny
128 < size ≤ 1024 →  SMALL  →  busca/crea zona small
size > 1024      →  LARGE  →  mmap directo, zona propia
```

Los bloques LARGE tienen su propia zona de un único bloque. Al liberarlos, la zona entera se devuelve al SO con `munmap`.

---

## Gestión de zonas vacías (reciclaje)

Cuando todos los bloques de una zona TINY/SMALL quedan libres, en lugar de llamar a `munmap`, la zona se **mueve a la lista de zonas vacías** (`empty_tiny_head` / `empty_small_head`). La próxima vez que se necesite una zona nueva del mismo tipo, se reutiliza una de esas zonas en lugar de llamar a `mmap` de nuevo.

Esto reduce la presión sobre el kernel y mejora el rendimiento en patrones alloc/free repetitivos.
