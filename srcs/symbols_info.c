
#include "ft_nm.h"
#include "symbols_info.h"
#include "utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

void
init_symbols_info(t_ctx* ctx, t_file* file, void* s_hdr, bool is_x64) {
	uint64_t	s_hdr_size;
	uint64_t	s_hdr_entsize;

	shdrcpy_size(&s_hdr_size, s_hdr, is_x64);
	shdrcpy_entsize(&s_hdr_entsize, s_hdr, is_x64);
	ctx->symbols_info_len = s_hdr_size / s_hdr_entsize;
	ctx->symbols_info = malloc(sizeof(t_symbol_info) * ctx->symbols_info_len);
	if (!ctx->symbols_info) {
		munmap(file->buffer, file->size);
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		exit(1);
	}
}

void
add_symbol_info(t_ctx* ctx, uint64_t value, char type, char* name) {
	ctx->symbols_info[ctx->symbols_info_len].symbol_value = value;
	ctx->symbols_info[ctx->symbols_info_len].symbol_type = type;
	ctx->symbols_info[ctx->symbols_info_len].symbol_name = name;
	ctx->symbols_info[ctx->symbols_info_len].need_to_be_display = (type != '*');
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
sort_symbols_name(t_ctx* ctx) {
	size_t	idx_lower;

	for (size_t i = 0; i < ctx->symbols_info_len; ++i) {
		idx_lower = i;
		for (size_t j = i + 1; j < ctx->symbols_info_len; j++)
			idx_lower = (strcmp(ctx->symbols_info[idx_lower].symbol_name, ctx->symbols_info[j].symbol_name) > 0) ? j : idx_lower;
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
	sort_symbols_name(ctx);
	sort_symbols_value(ctx);
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
