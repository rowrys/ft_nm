
#include "file.h"
#include "ft_nm.h"
#include "parse_elf.h"

#include <elf.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static void
parse_elf(t_ctx* ctx, t_file* file, char* file_path) {
	static uint16_t const	et_rel = ET_REL;
	static uint16_t const	et_dyn = ET_DYN;
	bool					is_x64;
	size_t					nb_phdr;
	size_t					nb_shdr;

	if (!is_elf(file)) {
		dprintf(STDERR_FILENO, "nm: %s: file format not recognized\n", file_path);
		ctx->exit_code = 1;
		return ;
	}
	is_x64 = (file->buffer[EI_CLASS] == ELFCLASS64);
	if (file->buffer[EI_CLASS] != ELFCLASS32 && !is_x64) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid class */
		ctx->exit_code = 1;
		return ;
	}
	if (!memcmp(&file->buffer[OFFSET_EHDR(is_x64, e_type)], &et_rel, 2) && !memcmp(&file->buffer[OFFSET_EHDR(is_x64, e_type)], &et_dyn, 2)) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid file format not (.so .o x86(_64)) */
		ctx->exit_code = 1;
		return ;
	}
	memcpy(&nb_phdr, &file->buffer[OFFSET_EHDR(is_x64, e_phnum)], 2);
	memcpy(&nb_shdr, &file->buffer[OFFSET_EHDR(is_x64, e_shnum)], 2);
	printf("%s:\n", file_path);
	return ;
}

void
parse_all_elf(t_ctx* ctx) {
	t_file	file;

	for (size_t i = 0; i < ctx->nb_binary; ++i) {
		map_file(&file, ctx->binary_file_path[i]);
		parse_elf(ctx, &file, ctx->binary_file_path[i]);
		munmap(file.buffer, file.size);
	}
}
