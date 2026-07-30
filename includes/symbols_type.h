#ifndef SYMBOLS_TYPE_H
# define SYMBOLS_TYPE_H

# include "parse_elf.h"

char	get_symbos_type(t_elf_info* elf_info, t_file* file, void* sym_hdr);

static inline char
convert_gloal_symbol(char c, bool is_global) {
	return (!is_global ? c : c - 32);
}

#endif
