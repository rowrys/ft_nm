#include "parse_elf.h"
#include "utils.h"

#include <sys/mman.h>
#include <stdlib.h>

static inline void
out_of_range(t_ctx* ctx) {
	dprintf(STDERR_FILENO, MSG_ERROR_OUT_OF_RANGE, ctx->current_binary_file_path);
	munmap(ctx->file->buffer, ctx->file->size);
	free(ctx->binary_file_path);
	free(ctx->symbols_info);
	exit(1);
}

static bool
check_elf_hdr(t_ctx* ctx) {
	static uchar const		elfclass32 = ELFCLASS32;
	static uint16_t const	et_rel = ET_REL;
	static uint16_t const	et_dyn = ET_DYN;
	bool					is_x64;

	if (!is_elf(ctx->file)) {
		dprintf(STDERR_FILENO, "nm: %s: file format not recognized\n", ctx->current_binary_file_path);
		ctx->exit_code = 1;
		return (1);
	}
	is_x64 = *((uchar*)ptr_add(ctx->file->buffer, EI_CLASS)) == (uchar)ELFCLASS64;
	if (memcmp(ptr_add(ctx->file->buffer, EI_CLASS), &elfclass32, sizeof(uchar)) && !is_x64) {
		dprintf(STDERR_FILENO, ERROR_MSG_CLASS); /* Invalid class */
		ctx->exit_code = 1;
		return (1);
	}
	if (!memcmp(ptr_add(ctx->file->buffer, OFFSET_EHDR(is_x64, e_type)), &et_rel, 2)
			&& !memcmp(ptr_add(ctx->file->buffer, OFFSET_EHDR(is_x64, e_type)), &et_dyn, 2)) {
		dprintf(STDERR_FILENO, ERROR_MSG_FORMAT); /* Invalid file format not (.so .o x86(_64)) */
		ctx->exit_code = 1;
		return (1);
	}
	return (0);
}

#define ELF_PARSE
#include "template/parse_elf_impls.h"

static void
parse_elf(t_ctx* ctx) {
	if (check_elf_hdr(ctx))
		return ;
	if (ctx->nb_binary != 1)
		printf("%s:\n", ctx->current_binary_file_path);
	if (*((uchar*)ptr_add(ctx->file->buffer, EI_CLASS)) == (uchar)ELFCLASS64)
		parse_section_hdr_elf64(ctx);
	else
		parse_section_hdr_elf32(ctx);
}

void
parse_all_elf(t_ctx* ctx) {
	t_file	file;

	for (size_t i = 0; i < ctx->nb_binary; ++i) {
		ctx->current_binary_file_path = ctx->binary_file_path[i];
		if (map_file(&file, ctx->current_binary_file_path)) {
			if (i != ctx->nb_binary - 1 && ctx->nb_binary != 1)
				printf("\n");
			continue ;
		}
		ctx->file = &file;
		parse_elf(ctx);
		display_symbols_info(ctx);
		if (i != ctx->nb_binary - 1 && ctx->nb_binary != 1)
			printf("\n");
		munmap(file.buffer, file.size);
		free(ctx->symbols_info);
		ctx->symbols_info = NULL;
		ctx->symbols_info_len = 0;
	}
}
