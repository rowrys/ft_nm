#include "ft_nm.h"

#include <ctype.h>
#include <string.h>

#define ELF_SYMBOLS_INFO
#include "template/parse_elf_impls.h"

static bool
is_external_symbol(char symbol_type) {
	if (isupper(symbol_type))
		return (1);
	switch (symbol_type) {
		case 'i': case 'w': case 'v':
			return (1);
	}
	return (0);
}

void
add_symbol_info(t_ctx* ctx, uint64_t value, char type, char* name) {
	ctx->symbols_info[ctx->symbols_info_len].symbol_value = value;
	ctx->symbols_info[ctx->symbols_info_len].symbol_type = type;
	ctx->symbols_info[ctx->symbols_info_len].symbol_name = name;
	if (get_option_stat(ctx->options, OPT_U))
		ctx->symbols_info[ctx->symbols_info_len].need_to_be_display = (type == 'w' || type == 'U');
	else if (get_option_stat(ctx->options, OPT_G))
		ctx->symbols_info[ctx->symbols_info_len].need_to_be_display = is_external_symbol(type);
	else
		ctx->symbols_info[ctx->symbols_info_len].need_to_be_display = (type != SYMBOL_TYPE_NON_PRINTABLE);
	++ctx->symbols_info_len;
}

static void
swapn(void* addr1, void* addr2, uint16_t size) {
	char	tmp[size];

	memcpy(tmp, addr1, size);
	memcpy(addr1, addr2, size);
	memcpy(addr2, tmp, size);
}

static void
sort_symbols_name(t_ctx* ctx, bool swap) {
	size_t	idx_lower;
	bool	cond_result;

	for (size_t i = 0; i < ctx->symbols_info_len; ++i) {
		idx_lower = i;
		for (size_t j = i + 1; j < ctx->symbols_info_len; j++) {
			cond_result = strcmp(ctx->symbols_info[idx_lower].symbol_name, ctx->symbols_info[j].symbol_name) > 0; 
			cond_result = (swap) ? !cond_result : cond_result;
			idx_lower = (cond_result) ? j : idx_lower;
		}
		if (i != idx_lower)
			swapn(&ctx->symbols_info[i], &ctx->symbols_info[idx_lower], sizeof(t_symbol_info));
	}
}

static void
sort_symbols_value(t_ctx* ctx) {
	size_t	idx_lower;

	for (size_t i = 0; i < ctx->symbols_info_len; ++i) {
		idx_lower = i;
		for (size_t j = i + 1; j < ctx->symbols_info_len && !strcmp(ctx->symbols_info[idx_lower].symbol_name, ctx->symbols_info[j].symbol_name); ++j)
			idx_lower = (ctx->symbols_info[j].symbol_value < ctx->symbols_info[idx_lower].symbol_value) ? j : idx_lower;
		if (idx_lower != i)
			swapn(&ctx->symbols_info[i], &ctx->symbols_info[idx_lower], sizeof(t_symbol_info));
	}
}

void
display_symbols_info(t_ctx* ctx) {
	if (!ctx->symbols_info_len) {
		if (!ctx->exit_code)
			printf("nm: %s: no symbols\n", ctx->current_binary_file_path);
		return ;
	}
	if (!get_option_stat(ctx->options, OPT_P)) {
		sort_symbols_name(ctx, get_option_stat(ctx->options, OPT_R));
		sort_symbols_value(ctx);
	}
	for (size_t i = 0; i < ctx->symbols_info_len; ++i) {
		if (ctx->symbols_info[i].need_to_be_display) {
			if (ctx->symbols_info[i].symbol_type == 'U' || ctx->symbols_info[i].symbol_type == 'w')
				printf("                 ");
			else
				printf("%.16zx ", ctx->symbols_info[i].symbol_value);
			printf("%c ", ctx->symbols_info[i].symbol_type);
			printf("%s\n", ctx->symbols_info[i].symbol_name);
		}
	}
}
