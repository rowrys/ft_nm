 
#include "symbols_type.h"
#include "ft_nm.h"
#include "utils.h"
#include <stdint.h>



static char
get_weak_symbols(uint16_t sym_hdr_shndx, uchar sym_hdr_info) {
	switch (ELF_ST_TYPE(sym_hdr_info)) {
		case STT_FUNC: case STT_NOTYPE:
			return ('w' - (32 * (sym_hdr_shndx != SHN_UNDEF)));
		case STT_OBJECT:
			return ('v' - (32 * (sym_hdr_shndx != SHN_UNDEF)));
	}
	return ('?');
}

static char
get_gnu_symbols(uint16_t sym_hdr_shndx) {
	switch (sym_hdr_shndx) {
		case SHN_ABS:
			return ('A');
		case SHN_COMMON:
			return ('C');
	}
	return ('?');
}

static char
get_specifique_symbol_type(t_ctx* ctx, unsigned short shnum, uint16_t sym_hdr_shndx, uchar sym_hdr_info) {
	if (sym_hdr_info == STT_FILE) {
		if (get_option_stat(ctx->options, OPT_A))
			return ('a');
		return (SYMBOL_TYPE_NON_PRINTABLE);
	}
	if (sym_hdr_shndx > shnum)
		return (get_gnu_symbols(sym_hdr_shndx));
	if (ELF_ST_BIND(sym_hdr_info) == STB_WEAK)
		return (get_weak_symbols(sym_hdr_shndx, sym_hdr_info));
	if (ELF_ST_TYPE(sym_hdr_info) == STT_GNU_IFUNC)
		return ('i');
	return (0);
}

#define ELF_SYMBOLS
#include "template/parse_elf_impls.h"

