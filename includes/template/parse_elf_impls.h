#ifndef PARSE_ELF_IMPLS_H
# define PARSE_ELF_IMPLS_H

# include <elf.h>

/**
 * section headers (Elfx_Shdr) -> [s_hdr, s_symtab]
 * symbols headers (Elfx_Sym) -> [sym_hdr]
 */

# define ELF_POSTFIX _elf64
# define ELF_SHDR Elf64_Shdr
# define ELF_SYM Elf64_Sym
# include "parse_elf_gen.h"


# define ELF_POSTFIX _elf32
# define ELF_SHDR Elf32_Shdr
# define ELF_SYM Elf32_Sym
# include "parse_elf_gen.h"

#endif
