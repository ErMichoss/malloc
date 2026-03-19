# Flujo de `realloc`

## Diagrama general

```
realloc(ptr, size)
     │
     ├── ptr == NULL ─────────────────────────────────► malloc(size)
     │
     ├── size == 0 ───────────────────────────────────► free(ptr) + NULL
     │
     ├── find_zone_for_ptr → no encontrada ──────────► error + NULL
     │
     ├── aligned_size ≤ block->size ──► split si sobra ──► return ptr (in-place)
     │
     ├── bloque siguiente libre y suficiente ─────────► expandir in-place ──► return ptr
     │
     └── fallback ──► malloc(size) + memcpy + free(ptr) ──► return new_ptr
```

---

## Caso 1 — `ptr == NULL`

Comportamiento estándar: equivale a `malloc(size)`.

```c
if (!ptr)
    return (malloc(size));
```

---

## Caso 2 — `size == 0`

Equivale a `free(ptr)` y devuelve `NULL`.

```c
if (size == 0) {
    free(ptr);
    return (NULL);
}
```

---

## Caso 3 — Reducción o tamaño igual (in-place)

Se calcula el tamaño alineado a `sizeof(void *)`:

```c
aligned_size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
```

Si el bloque actual ya es suficientemente grande:

- Si sobra espacio para un bloque nuevo (`block->size >= aligned_size + BLOCK_OFFSET`), se llama a `split_block` para no desperdiciar memoria.
- Se devuelve el mismo `ptr` sin mover datos.

```
Antes:  [cabecera | datos (old_size) ─────────────────]
Después:[cabecera | datos (aligned_size) | nuevo libre ]
```

---

## Caso 4 — Expansión in-place (merge con siguiente bloque libre)

Si el bloque siguiente es libre y la suma de ambos es suficiente para el nuevo tamaño:

```c
block->size + block->next->size + BLOCK_OFFSET >= aligned_size
```

Se fusionan los dos bloques:

```c
block->size = block->size + aux_block->size + BLOCK_OFFSET;
block->next = aux_block->next;
```

Si tras el merge sobra espacio, se hace un split del sobrante.

Se devuelve el mismo `ptr`, sin copiar datos ni llamar a `malloc`/`free`. Esta es la optimización más importante de `realloc` para arrays que crecen incrementalmente.

```
Antes:
┌──────────────┬──────────────────┬────────────────────┐
│ t_block (A)  │  datos (old)     │ t_block (B, libre) │
└──────────────┴──────────────────┴────────────────────┘

Después:
┌──────────────┬────────────────────────────────────────┐
│ t_block (A)  │  datos (new_size, mismo ptr)           │
└──────────────┴────────────────────────────────────────┘
```

---

## Caso 5 — Fallback: nueva asignación

Si no es posible expandir in-place:

1. `malloc(size)` → nuevo bloque.
2. `ft_memcpy(new_ptr, ptr, block->size)` → copia los datos del bloque antiguo.
3. `free(ptr)` → libera el bloque anterior.
4. Devuelve `new_ptr`.

> ⚠️ Se copia `block->size` bytes (el tamaño **antiguo**), no `size`. Esto es correcto porque se copia exactamente lo que había, y el área extra del nuevo bloque queda sin inicializar (comportamiento estándar de `realloc`).

---

## Tabla resumen

| Condición | Estrategia | Copia de datos |
|-----------|-----------|----------------|
| `ptr == NULL` | `malloc(size)` | No aplica |
| `size == 0` | `free(ptr)` | No aplica |
| `aligned_size ≤ block->size` | In-place (split opcional) | ❌ No |
| Siguiente bloque libre y suficiente | Merge in-place (split opcional) | ❌ No |
| Ninguna de las anteriores | Nueva asignación + memcpy + free | ✅ Sí |

---

## Notas sobre alineamiento

El tamaño solicitado se alinea a `sizeof(void *)` (8 bytes en sistemas de 64 bits) antes de cualquier comparación:

```c
aligned_size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
```

Esto garantiza que los punteros devueltos estén correctamente alineados para cualquier tipo de dato.
