# Flujo de `free` y coalescencia

## Diagrama general

```
free(ptr)
     │
     ├── ptr == NULL ────────────────────────────────► return (no-op)
     │
     ├── find_zone_for_ptr(ptr) ── no encontrada ────► error + exit(1)
     │
     ├── bloque ya libre (is_free == true) ──────────► error + exit(1)
     │
     ├── type == LARGE ──► remove_large_zone + munmap ──► return
     │
     └── type == TINY/SMALL
               │
               ├── marcar is_free = true
               ├── coalescencia con bloque siguiente
               ├── coalescencia con bloque anterior
               └── ¿zona completamente libre?
                         ├── NO ──► return
                         └── SÍ ──► remove_zone_list + mover a empty list
```

---

## Paso 1 — Localizar la zona

`find_zone_for_ptr` recorre las listas `tiny_head`, `small_head` y `large_head` comprobando si el puntero cae dentro del rango de memoria de la zona:

```c
(void *)zone <= ptr && ptr < (void *)((char *)zone + zone->total_size)
```

Si no se encuentra ninguna zona que contenga el puntero, se trata de un puntero inválido o un doble `free` y el programa termina.

---

## Paso 2 — Obtener la cabecera del bloque

El puntero que recibe el usuario apunta al área de datos. Para acceder a la cabecera `t_block`:

```c
data_block = (t_block *)((char *)ptr - BLOCK_OFFSET);
```

Se verifica también que `is_free == false`. Si ya era libre, es un doble `free`.

---

## Paso 3 — Liberación LARGE

Para bloques LARGE, la zona contiene un único bloque. La liberación es inmediata:

1. `remove_large_zone` desenlaza la zona de `data.large_head`.
2. `munmap(zone, zone->total_size)` devuelve la memoria al SO.

No hay coalescencia porque no hay bloques vecinos.

---

## Paso 4 — Liberación TINY/SMALL con coalescencia

### 4a — Marcar como libre

```c
data_block->is_free = true;
```

### 4b — Coalescencia hacia adelante (merge con siguiente)

Si el bloque siguiente existe y también está libre, se fusionan:

```
Antes:
┌────────────┬─────────┬────────────┬─────────┐
│ t_block(A) │ datos A │ t_block(B) │ datos B │
│ is_free=T  │         │ is_free=T  │         │
└────────────┴─────────┴────────────┴─────────┘

Después:
┌────────────┬────────────────────────────────┐
│ t_block(A) │ datos A + cabecera B + datos B │
│ is_free=T  │                                │
└────────────┴────────────────────────────────┘
```

```c
data_block->size = data_block->size + aux_block->size + BLOCK_OFFSET;
data_block->next = aux_block->next;
```

### 4c — Coalescencia hacia atrás (merge con anterior)

Si el bloque anterior existe y también está libre, el bloque anterior absorbe al actual:

```c
aux_block->size = aux_block->size + data_block->size + BLOCK_OFFSET;
aux_block->next = data_block->next;
```

Tras esto, `data_block` apunta al bloque anterior fusionado (ahora el bloque raíz).

---

## Paso 5 — Detección de zona completamente libre

Se recorre la lista de bloques de la zona. Si **todos** están libres, la zona se recicla:

```
head_block = zone->head
while (head_block != NULL):
    if head_block->is_free == false → return   (zona todavía en uso)
    head_block = head_block->next

→ Zona completamente libre
```

### Reciclaje de zona

En lugar de llamar a `munmap`, la zona se **mueve a la lista de zonas vacías**:

```c
// Ejemplo para TINY:
zone->next = data.empty_tiny_head;
data.empty_tiny_head = zone;
```

La próxima llamada a `malloc` del mismo tipo puede reutilizar esta zona sin llamar a `mmap`.

---

## Detección de errores

| Situación | Acción |
|-----------|--------|
| `ptr == NULL` | Return silencioso (comportamiento estándar POSIX) |
| Puntero no encontrado en ninguna zona | `print_str` + `exit(1)` |
| Bloque ya marcado como libre | `print_str` + `exit(1)` |

Los mensajes de error se escriben directamente con `write` (sin usar el propio `malloc` en el path de error).
