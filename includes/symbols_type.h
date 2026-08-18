#ifndef SYMBOLS_TYPE_H
# define SYMBOLS_TYPE_H

# include "ft_nm.h"

char	get_symbos_type(t_ctx* ctx, void* sym_hdr);

static inline char
convert_gloal_symbol(char c, bool is_global) {
	return (!is_global ? c : c - 32);
}

#endif
