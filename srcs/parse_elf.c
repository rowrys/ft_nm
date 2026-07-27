
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

#include <endian.h>

static bool get_elf_info(t_elf_info* elf_info, t_file* file, t_ctx* ctx, char* file_path) {
	static uint16_t const	et_rel = ET_REL;
	static uint16_t const	et_dyn = ET_DYN;

	if (!is_elf(file)) {
		dprintf(STDERR_FILENO, "nm: %s: file format not recognized\n", file_path);
		ctx->exit_code = 1;
		return (1);
	}
	elf_info->is_x64 = (((uchar*)file->buffer)[EI_CLASS] == (uchar)ELFCLASS64);
	if (((uchar*)file->buffer)[EI_CLASS] != (uchar)ELFCLASS32 && !elf_info->is_x64) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid class */
		ctx->exit_code = 1;
		return (1);
	}
	if (!memcmp(&((uchar*)file->buffer)[OFFSET_EHDR(elf_info->is_x64, e_type)], &et_rel, 2)
			&& !memcmp(&((uchar*)file->buffer)[OFFSET_EHDR(elf_info->is_x64, e_type)], &et_dyn, 2)) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid file format not (.so .o x86(_64)) */
		ctx->exit_code = 1;
		return (1);
	}
	memcpy(&elf_info->nb_phdr, ((uchar*)file->buffer) + OFFSET_EHDR(elf_info->is_x64, e_phnum), 2);
	memcpy(&elf_info->nb_shdr, ((uchar*)file->buffer) + OFFSET_EHDR(elf_info->is_x64, e_shnum), 2);
	memcpy(&elf_info->size_phdr, ((uchar*)file->buffer) + OFFSET_EHDR(elf_info->is_x64, e_phentsize), 2);
	memcpy(&elf_info->size_shdr, ((uchar*)file->buffer) + OFFSET_EHDR(elf_info->is_x64, e_shentsize), 2);
	return (0);
}

static void
parse_elf(t_ctx* ctx, t_file* file, char* file_path) {
	t_elf_info	elf_info;

	if (get_elf_info(&elf_info, file, ctx, file_path))
		return ;

	printf("nb programme hdr{%d}\n", elf_info.nb_phdr);
	printf("size programme hdr{%d}\n", elf_info.size_phdr);
	printf("nb section hdr{%d}\n", elf_info.nb_shdr);
	printf("size section hdr{%d}\n", elf_info.size_shdr);
	// parse_shdr(file, elf_info.is_x64, elf_info.nb_phdr);
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
