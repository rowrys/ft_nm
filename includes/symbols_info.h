#ifndef SYMBOLS_INFO_H
# define SYMBOLS_INFO_H

# include "file.h"

# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

typedef struct s_ctx t_ctx;

typedef struct s_symbol_info {
	uint64_t	symbol_value;
	char		symbol_type;
	char*		symbol_name;
	bool		need_to_be_display;
}	t_symbol_info;

void	init_symbols_info(t_ctx* ctx, t_file* file, void* s_hdr, bool is_x64);
void	add_symbol_info(t_ctx* ctx, uint64_t value, char type, char* name);
void	display_symbols_info(t_ctx* ctx);

#endif
