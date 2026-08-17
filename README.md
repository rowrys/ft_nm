# ft_nm

A lightweight reimplementation of [`nm`](https://www.man7.org/linux/man-pages/man1/nm.1.html) written in C.

**ft_nm** parses ELF binaries directly to extract, classify, sort and display their symbols.
The project focuses on understanding the ELF file format and implementing low-level binary parsing without relying on a dedicated ELF parsing library.

> **42 project — Low-level C / ELF parsing / Systems programming**

---

## 📖 Overview

`nm` is a Unix utility used to display the symbols contained in object files, executables and shared libraries.

The goal of **ft_nm** is to reproduce a limited but representative subset of its behavior while implementing the underlying ELF parsing logic from scratch.

The program reads an ELF file, navigates through its headers and sections, locates the symbol table, interprets each symbol and produces an output similar to `nm`.

### Main objectives

* Understand the **ELF binary format**.
* Parse ELF structures directly from a file mapped in memory.
* Support both **ELF32 and ELF64** binaries.
* Locate and parse the `.symtab` section.
* Resolve symbol names through the associated string table.
* Determine the type of each symbol.
* Filter symbols according to command-line options.
* Sort symbols to reproduce `nm`like output.

---

## 🧠 What I learned

This project was mainly an exercise in **systems programming and binary format analysis**.

### ELF internals

I learned how an ELF file is organized and how its different structures are related:

```text
ELF Header
    │
    ├── Program Headers
    │
    └── Section Headers
            │
            ├── .symtab
            │      └── Symbol entries
            │
            └── String Table
                   └── Symbol names
```

The parser uses information from the ELF header to locate and interpret the section headers and symbol table.

### Binary parsing

Instead of treating the input as a text file, the project accesses the binary data directly in memory.

The implementation handles differences between:

* ELF32
* ELF64
* 32-bit and 64-bit structure layouts
* different field sizes and offsets

### Memory mapping

Files are loaded using `mmap()` rather than being read into a manually allocated buffer.

This provides direct access to the binary data and makes pointer-based parsing possible.

### Symbol analysis

I implemented symbol classification based on information such as:

* symbol binding;
* symbol type;
* section type;
* section flags;
* special section indexes.

The implementation handles several symbol categories, including:

* text symbols;
* data symbols;
* BSS symbols;
* read-only symbols;
* undefined symbols;
* absolute symbols;
* common symbols;
* weak symbols;
* GNU IFUNC symbols.

### Command-line parsing

The project also implements a subset of `nm` options:

| Option | Behavior                       |
| ------ | ------------------------------ |
| `-a`   | Display debugger-only symbols  |
| `-g`   | Display external symbols       |
| `-p`   | Do not sort symbols            |
| `-r`   | Reverse the sort order         |
| `-u`   | Display undefined symbols only |

Some options have priority rules, for example `-u` overriding `-a` and `-g`, similarly to the behavior targeted by the project.

---

## 🔍 How it works

The general execution flow is:

```text
Command-line arguments
        │
        ▼
   Parse options
        │
        ▼
    Open file
        │
        ▼
      mmap()
        │
        ▼
   Validate ELF
        │
        ▼
    Read ELF Header
        │
        ▼
  Locate Section Headers
        │
        ▼
    Find .symtab
        │
        ▼
  Read symbol entries
        │
        ├── Resolve symbol name
        │
        ├── Determine symbol type
        │
        └── Apply filters
        │
        ▼
     Sort symbols
        │
        ▼
      Display
```

This separation allows the project to keep the main stages of the parser relatively independent:

* argument handling;
* file mapping;
* ELF parsing;
* symbol type detection;
* symbol storage;
* filtering;
* sorting;
* output.

---

## 🏗️ Architecture

The implementation is organized around several responsibilities.

### `arguments.c`

Handles command-line arguments and stores the selected options in a bitmask.

### `file.c`

Responsible for opening files and mapping them into memory using `mmap()`.

### `parse_elf.c`

Contains the main ELF parsing logic:

* ELF validation;
* ELF32 / ELF64 detection;
* extraction of ELF metadata;
* section header traversal;
* symbol table discovery;
* symbol entry parsing.

### `symbols_type.c`

Determines the character used to represent each symbol according to its ELF metadata and section information.

### `symbols_info.c`

Stores parsed symbols and handles:

* filtering;
* alphabetical sorting;
* value-based sorting;
* output formatting.

### `utils.h`

Contains low-level helpers used to access ELF fields while accounting for the differences between ELF32 and ELF64 layouts.

---

## 🌳 Project Structure

```text
.
├── Makefile
├── README.md
├── includes
│   ├── arguments.h
│   ├── file.h
│   ├── ft_nm.h
│   ├── parse_elf.h
│   ├── symbols_info.h
│   ├── symbols_type.h
│   └── utils.h
└── srcs
    ├── arguments.c
    ├── file.c
    ├── nm.c
    ├── parse_elf.c
    ├── symbols_info.c
    └── symbols_type.c
```

---

## ⚙️ Build

The project is compiled with:

```bash
cc -MP -MMD -Wall -Wextra -Werror -g
```

### Available Make commands

```bash
make
```

Build the project.

```bash
make clean
```

Remove generated object files.

```bash
make fclean
```

Remove object files and the executable.

```bash
make re
```

Clean and rebuild the project.

---

## 🚀 Usage

Run the program on the default `a.out` file:

```bash
./ft_nm
```

Or specify one or more binaries:

```bash
./ft_nm binary
./ft_nm binary1 binary2
```

### Options

```bash
./ft_nm -a binary
./ft_nm -g binary
./ft_nm -p binary
./ft_nm -r binary
./ft_nm -u binary
```

Options can also be combined:

```bash
./ft_nm -gr binary
```

The output is designed to follow the general format and behavior of `nm` within the scope of the project.

---

## 🧪 Example

For an ELF binary containing symbols:

```bash
./ft_nm ./binary
```

The output follows the classic `nm`-style format:

```text
0000000000001139 T main
0000000000004010 D global_data
0000000000004020 B global_bss
                 U printf
```

The exact output depends on the symbols contained in the analyzed binary and the selected options.

---

## 🛠️ Technical Highlights

### Direct ELF parsing

The project does not use a high-level ELF parsing library. ELF structures are interpreted directly from the memory-mapped file.

### ELF32 / ELF64 support

The parser adapts field offsets and sizes depending on the ELF class.

```c
ELFCLASS32
ELFCLASS64
```

This required taking into account differences between structures such as:

```text
Elf32_Ehdr / Elf64_Ehdr
Elf32_Shdr / Elf64_Shdr
Elf32_Sym  / Elf64_Sym
```

### Memory-mapped files

Files are mapped with:

```c
mmap()
```

and released with:

```c
munmap()
```

This allows the parser to navigate the binary directly through memory offsets.

### Symbol classification

Symbol types are derived from ELF metadata rather than hardcoded from symbol names.

For example, section flags are used to distinguish categories such as:

```text
.text  → T / t
.data  → D / d
.bss   → B / b
.rodata → R / r
```

along with special cases such as undefined, weak, absolute and common symbols.

---

## 📚 Resources

The project was developed using the following references:

* [`nm(1)` — Linux manual](https://www.man7.org/linux/man-pages/man1/nm.1.html)
* [`elf(5)` — Linux manual](https://www.man7.org/linux/man-pages/man5/elf.5.html)
* [System V ABI — ELF Specification](https://refspecs.linuxfoundation.org/elf/elf.pdf)

---

## 🤖 AI Usage

AI was used as a **learning and verification tool**, not as a source of implementation code.

I mainly used AI to:

* generate questions to test my understanding of the ELF format;
* challenge my reasoning about ELF structures and symbol tables;
* suggest files and edge cases to use for testing;
* review the README structure and presentation.

The implementation and debugging process were done by understanding and writing the code myself.

---

## 🎯 What this project demonstrates

**ft_nm** demonstrates practical experience with:

* **C programming**
* **Systems programming**
* **Binary file parsing**
* **ELF internals**
* **Memory mapping with `mmap`**
* **Pointer arithmetic**
* **Bit manipulation**
* **32-bit / 64-bit data structures**
* **Command-line argument parsing**
* **Symbol tables**
* **Memory management**
* **Error handling**
* **Algorithmic sorting**

The project was particularly valuable for understanding how tools such as `nm` can inspect a compiled binary by navigating its underlying file format rather than relying on source-level information.

---

## 📌 Project Status

The current implementation focuses on a restricted subset of `nm` functionality required by the project.

The goal was not to reproduce every feature of GNU `nm`, but to build a working symbol analyzer while gaining a deeper understanding of the ELF format and low-level binary parsing.
