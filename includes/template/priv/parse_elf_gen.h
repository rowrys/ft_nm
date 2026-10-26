#ifndef ELF_POSTFIX
# error ELF_POSTFIX is not defined
#endif
#ifndef ELF_SHDR
# error ELF_SHDR is not defined
#endif
#ifndef ELF_SYM
# error ELF_SYM is not defined
#endif
#ifndef ELF_EHDR
# error Elf_EHDR is not defined
#endif

#define ELF_GLUE_I(x, y) x##y
#define ELF_GLUE(x, y) ELF_GLUE_I(x, y)
#define ELF_NAMED(x) ELF_GLUE(x, ELF_POSTFIX)

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ELF_INFO ((ELF_EHDR*)(ctx->file->buffer))

#ifndef ELF_SYMBOLS_INFO
# include "elf_symbols_info.h"
#endif
#ifdef ELF_SYMBOLS
# include "elf_symbols.h"
#endif
#ifdef ELF_PARSE
# include "elf_parse.h"
#endif

#undef ELF_GLUE_I
#undef ELF_GLUE
#undef ELF_NAMED

#undef ELF_INFO

#undef ELF_POSTFIX
#undef ELF_SHDR
#undef ELF_SYM
#undef ELF_EHDR
