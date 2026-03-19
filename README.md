# ft_malloc

> Reimplementación de `malloc`, `free` y `realloc` en C como biblioteca dinámica compartida (`.so`), siguiendo las especificaciones del proyecto de 42.

---

## 📋 Descripción

`ft_malloc` es una implementación completa del gestor de memoria dinámica de C. Reemplaza las funciones estándar de la libc utilizando zonas de memoria pre-asignadas con `mmap`, optimizando el rendimiento mediante la reutilización de bloques libres y la fusión de bloques adyacentes (coalescencia).

El allocator divide las asignaciones en tres categorías:

| Tipo | Tamaño máximo | Estrategia |
|------|--------------|------------|
| `TINY` | ≤ 128 bytes | Zonas pre-asignadas con 100 bloques |
| `SMALL` | ≤ 1024 bytes | Zonas pre-asignadas con 100 bloques |
| `LARGE` | > 1024 bytes | Asignación directa con `mmap` |

---

## 🚀 Compilación y uso

### Requisitos

- `gcc`
- `make`
- Sistema Linux o macOS

### Build

```bash
make
```

Genera `libft_malloc_<ARCH>_<OS>.so` y el symlink `libft_malloc.so`.

### Compilar los tests

```bash
make test
./test_malloc_app
```

### Usar la librería con un programa existente

```bash
# Linux
LD_PRELOAD=./libft_malloc.so ./tu_programa

# macOS
DYLD_INSERT_LIBRARIES=./libft_malloc.so ./tu_programa
```

---

## 🐛 Modo debug

Activa el log detallado con la variable de entorno `MALLOC_VERBOSE`:

```bash
MALLOC_VERBOSE=1 ./test_malloc_app
```

Ejemplo de salida:

```
DEBUG: Malloc en block LARGE. Dirección Zone: 0x7f..., Dirección block: 0x7f..., Tamaño: 2048
DEBUG: Free en block TINY/SMALL. Dirección liberada: 0x7f...
DEBUG: Reutilizando zona vacía en 0x7f...
```

---

## 📦 Funciones públicas

### Funciones estándar

```c
void  *malloc(size_t size);
void   free(void *ptr);
void  *realloc(void *ptr, size_t size);
```

### Funciones de inspección

```c
void   show_alloc_mem(void);     // Muestra bloques asignados por zona
void   show_alloc_mem_ex(void);  // Muestra bloques con volcado hexadecimal
```

---

## 🗂️ Estructura del proyecto

```
ft_malloc/
├── Makefile
├── incl/
│   └── malloc.h               # Estructuras, macros y prototipos
├── src/
│   ├── malloc.c               # malloc() + lógica de zonas LARGE
│   ├── free.c                 # free() + coalescencia de bloques
│   ├── realloc.c              # realloc() con expansión in-place
│   ├── blocks.c               # Búsqueda, split y reutilización de bloques
│   ├── memory_zone.c          # show_alloc_mem()
│   ├── memory_zone_bonus.c    # show_alloc_mem_ex() con hex dump
│   ├── globals.c              # Variables globales y MALLOC_VERBOSE
│   ├── aux.c                  # Funciones auxiliares y gestión de zonas
│   └── auxTwo.c               # ft_memcpy
├── lib/
│   └── printf/                # ft_printf propio (sin libc)
└── test/
    └── test_malloc.c
```

---

## ⚙️ Makefile targets

| Target | Descripción |
|--------|-------------|
| `make` | Compila la shared library |
| `make test` | Compila el ejecutable de pruebas |
| `make clean` | Elimina objetos |
| `make fclean` | Elimina objetos y la librería |
| `make re` | `fclean` + `all` |

---

## 📚 Documentación técnica

- [Arquitectura y estructuras de datos](docs/architecture.md)
- [Flujo de malloc](docs/malloc_flow.md)
- [Flujo de free y coalescencia](docs/free_flow.md)
- [Flujo de realloc](docs/realloc_flow.md)

---

## 📄 Licencia

Proyecto educativo — 42 Madrid.
