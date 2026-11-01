# ft_nm

A from-scratch reimplementation of the Unix `nm` tool in C. It reads ELF binaries (32-bit and 64-bit, x86 / x86-64) **directly from their raw bytes**, with no parsing library, and lists their symbol tables the way GNU `nm` does.

Built as part of the systems programming curriculum at **42 school**.

---

## Why this project is interesting

`nm` looks simple from the outside, but a correct implementation means understanding how an executable is actually laid out on disk and handling untrusted binary input safely.

- **Binary format parsing** — walking ELF headers, section header tables, string tables and symbol tables by hand.
- **Defensive programming** — the input is an arbitrary file, so section header accesses are checked against the real file size instead of trusting the file's own metadata.
- **Low-level C** — `mmap`, pointer arithmetic on raw buffers, bitmask option handling, and no dependency beyond libc and `<elf.h>`.

## Features

- Supports **ELF32 and ELF64** (x86 / x86-64): object files, shared libraries and executables.
- Handles **several files in one call** (prints a `file:` header for each, like `nm`).
- Defaults to `a.out` when no file is given.
- Symbol type letters computed from section type/flags and symbol binding (`U`, `w`/`W`, `v`/`V`, `i`, `A`, `C`, `a`, plus section-based letters).
- Options, with their long forms:

| Option | Long form | Effect |
|--------|-----------|--------|
| `-a` | `--debug-syms` | Show debugger-only symbols |
| `-g` | `--extern-only` | Show only external symbols |
| `-p` | `--no-sort` | Keep symbols in table order |
| `-r` | `--reverse-sort` | Reverse the sort order |
| `-u` | `--undefined-only` | Show only undefined symbols |

  Options can be combined (`-gr`) and follow the same priority rules as the original (`-u` overrides `-g` and `-a`; `-g` overrides `-a`).
- Clear error messages on invalid input: missing file, not an ELF file, truncated file, unknown option.

## Build

```bash
make          # builds ./ft_nm
make debug    # builds with -D DEBUG
make gdb      # builds and opens gdb in TUI mode
make re       # full rebuild
```

Compiled with `-Wall -Wextra -Werror -std=c11 -pedantic-errors`. Object files go to `.build/`, and header dependencies are tracked automatically (`-MMD -MP`), so editing a `.h` rebuilds exactly what depends on it.

## Usage

```bash
./ft_nm [option(s)] [file(s)]
./ft_nm ./ft_nm
./ft_nm -g libfoo.so
./ft_nm -ur a.out other.o
```

Output format: `address`, `type letter`, `name` (undefined symbols have no address).

```
0000000000001129 T main
                 U printf
```

## How it works

1. **Argument parsing** — options are stored as bit flags in a single integer, and the list of files is collected separately.
2. **File mapping** — each file is opened read-only and mapped with `mmap(PROT_READ, MAP_PRIVATE)`: no `read()` loop and no copy of the file in memory.
3. **ELF validation** — checks the magic number and the class (32/64-bit) before touching anything else.
4. **Symbol extraction** — locates the section header table, finds the symbol table and the string table linked to it (`sh_link`), then resolves each symbol name (`st_name`) and type.
5. **Filtering and sorting** — symbols are stored in one structure (value, type letter, name, display flag), filtered according to the options, sorted by name then by value, and printed.

### Design choices

- **One shared code path for 32-bit and 64-bit.** Rather than writing the parser twice, the ELF32 and ELF64 variants are generated from a single header template. `parse_elf_impls.h` includes the same generic implementation twice with different macro definitions (`ELF_POSTFIX`, `ELF_SHDR`, `ELF_SYM`, `ELF_EHDR`), and token pasting (`ELF_NAMED(x)`) turns each function into `x_elf32` / `x_elf64`, bound to the matching `Elf32_*` / `Elf64_*` types. The two versions stay identical in logic, there is no copy-paste to keep in sync, and a missing macro is a compile-time error.
- **Zero-copy.** The file is mapped once and symbol names are pointers into that mapping, so no string is duplicated.
- **Options as a bitmask**, read and written through small `static inline` accessors, with explicit priority rules when options conflict.
- **One context struct** (`t_ctx`) carries all the program state (options, files, symbols, exit code), so there are no globals and cleanup happens in one place.
- **Bounds checks as small inline helpers** (`check_offset`, `check_addr`, `next_hdr`) so that safety checks stay readable and are not skipped by accident.

## Project structure

```
.
├── Makefile
├── includes/
│   ├── arguments.h      # option parsing
│   ├── file.h           # file mapping
│   ├── ft_nm.h          # shared context, option flags, pointer helpers
│   ├── parse_elf.h      # ELF validation and parsing entry points
│   ├── symbols_info.h   # symbol storage, sorting, display
│   ├── symbols_type.h   # type-letter resolution
│   ├── utils.h          # bounds-checking helpers
│   └── template/        # shared 32/64-bit implementation
└── srcs/
    ├── nm.c             # main
    ├── arguments.c
    ├── file.c
    ├── parse_elf.c
    ├── symbols_info.c
    └── symbols_type.c
```

## What I learned


- A symbol's name is not stored in the symbol: it is an index into a **string table located through `sh_link`**.
- `SHT_SYMTAB` holds the full symbol table, while `SHT_DYNSYM` only holds the dynamic subset, which is why it is the one to look for.
- Sorting depends on the **locale**.
- **C can do generics through the preprocessor**: one parser included twice with different macros and token pasting (`ELF_NAMED`) generates both the ELF32 and ELF64 versions, at the cost of code that is harder to read.

## Known limitations

- x86 / x86-64 little-endian only; other architectures and big-endian files are out of scope.
- Archives (`.a`) are not supported.
- Implements the five options listed above, not the full GNU `nm` option set.
- Addresses are always printed on 16 hex digits, including for 32-bit files (GNU `nm` uses 8 digits there).
- Sorting uses a selection sort (O(n²)), which is fine for typical binaries but would be the first thing to replace with `qsort` or a merge sort for very large symbol tables.

## AI Usage

AI was used as a **learning and verification tool**, not as a source of implementation code.

I mainly used AI to:

* generate questions to test my understanding of the ELF format;
* challenge my reasoning about ELF structures and symbol tables;
* suggest files and edge cases to use for testing;
* help draft and edit this README

The implementation and debugging process were done by understanding and writing the code myself.
