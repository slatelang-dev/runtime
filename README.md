# Slate Runtime

Garbage collection, object system, and panic handling for compiled Slate programs.

## Modules

- **gc/** - Garbage collector
- **objects/** - Value representation
- **panic.slt** - Error handling

## Usage

Include `slate_runtime.h` in generated C code. Provides:
- Memory allocation (`slate_alloc`, `slate_free`)
- String operations (`slate_concat`, `slate_trim`, etc.)
- Collections (`slate_len`, `slate_get`, `slate_table_*`)
- File I/O (`slate_file_read`, `slate_file_write`)