# Flujo de `malloc`

## Diagrama general

```
malloc(size)
     │
     ├── size == 0 ──────────────────────────────────► return NULL
     │
     ├── size ≤ 128 (TINY) ──► find_or_create_block(size, TINY)
     │                                  │
     ├── size ≤ 1024 (SMALL) ──► find_or_create_block(size, SMALL)
     │
     └── size > 1024 (LARGE) ──► asignación directa con mmap
                                          │
                                          └── return ptr
```

---

## Flujo TINY / SMALL — `find_or_create_block`

La función sigue un orden estricto de **prioridades**:

### Prioridad 1 — Reutilizar zona vacía (`reuse_empty_zone`)

Comprueba si hay alguna zona completamente libre guardada en `empty_tiny_head` / `empty_small_head`. Si existe:

1. Saca la zona de la lista de vacías.
2. La mueve al frente de la lista activa (`tiny_head` / `small_head`).
3. Busca un bloque libre en ella con `find_and_split_block`.

Esto evita una llamada a `mmap` cuando el patrón de uso es alloc/free repetitivo.

### Prioridad 2 — Buscar bloque libre en zonas existentes

Recorre la lista activa (`tiny_head` o `small_head`) buscando un bloque con `is_free == true` y `size >= size` solicitado.

Si lo encuentra, llama a `split_block` para dividirlo si sobra espacio suficiente.

### Prioridad 3 — Crear una zona nueva (`create_new_zone`)

Si no hay bloques disponibles:

1. Calcula el tamaño total: `(size + BLOCK_OFFSET) * BLOCKS_PER_ZONE`.
2. Redondea al múltiplo de página: `round_up_to_page_size(total_size)`.
3. Llama a `mmap(NULL, aligned_size, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0)`.
4. Coloca `t_zone` al inicio del bloque mapeado.
5. Coloca el primer `t_block` justo después de `t_zone`.
6. Añade la zona a la lista correspondiente.

```
mmap devuelve ──► [ t_zone | t_block (libre, size = todo el resto) ]
```

---

## Flujo LARGE — asignación directa

Para tamaños mayores de 1024 bytes, `malloc` crea una zona propia de un único bloque:

```c
total_size   = size + BLOCK_OFFSET + sizeof(t_zone);
aligned_size = round_up_to_page_size(total_size);
mmap(...)
```

Layout en memoria:

```
[ t_zone ][ t_block (is_free=false, type=LARGE) ][ datos... ]
▲ base mmap
```

La zona se añade a `data.large_head`. Al liberar, se llama a `munmap` sobre toda la zona.

---

## `split_block` — División de bloques

Cuando se encuentra un bloque libre más grande que lo solicitado, se divide en dos:

```
Antes:
┌──────────────────────────────────────────────────┐
│ t_block  │ datos (old_size bytes, libre)          │
└──────────────────────────────────────────────────┘

Después (si old_size > size + BLOCK_OFFSET):
┌────────────────────────┬───────────────────────────┐
│ t_block │ datos(size)  │ t_block │ resto (libre)    │
└────────────────────────┴───────────────────────────┘
```

Condición de split: `old_size > size + BLOCK_OFFSET` (debe haber espacio para la cabecera del nuevo bloque).

Tras el split, si el bloque siguiente al nuevo también está libre, se fusionan inmediatamente (coalescencia proactiva).

---

## Valor de retorno

En todos los casos, el puntero devuelto al usuario es:

```c
return (void *)((char *)block + BLOCK_OFFSET);
```

Es decir, apunta **justo al inicio del área de datos**, saltándose la cabecera `t_block`.

---

## Diagrama de memoria tras varias asignaciones TINY

```
data.tiny_head
      │
      ▼
  ┌─────────┐
  │ t_zone  │──next──► NULL
  ├─────────┤
  │ t_block │ is_free=false, size=64   ◄── malloc(64) → ptr1
  ├─────────┤
  │ t_block │ is_free=false, size=32   ◄── malloc(32) → ptr2
  ├─────────┤
  │ t_block │ is_free=true,  size=...  ◄── espacio disponible
  └─────────┘
```
