
#include "file.h"
#include "ft_nm.h"
#include "parse_elf.h"

#include <elf.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <endian.h>

static bool
check_elf_hdr(t_file* file, t_ctx* ctx, char* file_path) {
	static uint16_t const	et_rel = ET_REL;
	static uint16_t const	et_dyn = ET_DYN;
	bool					is_x64;

	if (!is_elf(file)) {
		dprintf(STDERR_FILENO, "nm: %s: file format not recognized\n", file_path);
		ctx->exit_code = 1;
		return (1);
	}
	is_x64 = (((uchar*)file->buffer)[EI_CLASS] == (uchar)ELFCLASS64);
	if (((uchar*)file->buffer)[EI_CLASS] != (uchar)ELFCLASS32 && !is_x64) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid class */
		ctx->exit_code = 1;
		return (1);
	}
	if (!memcmp(&((uchar*)file->buffer)[OFFSET_EHDR(is_x64, e_type)], &et_rel, 2)
			&& !memcmp(&((uchar*)file->buffer)[OFFSET_EHDR(is_x64, e_type)], &et_dyn, 2)) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid file format not (.so .o x86(_64)) */
		ctx->exit_code = 1;
		return (1);
	}
	return (0);
}

static void
get_elf_info(t_elf_info* elf_info, t_file* file) {
	elf_info->is_x64 = (((uchar*)file->buffer)[EI_CLASS] == (uchar)ELFCLASS64);
	memcpy(&elf_info->nb_phdr, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_phnum)), 2);
	memcpy(&elf_info->nb_shdr, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shnum)), 2);
	memcpy(&elf_info->size_phdr, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_phentsize)), 2);
	memcpy(&elf_info->size_shdr, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shentsize)), 2);
	memcpy(&elf_info->shstrndx, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shstrndx)), 2);
	memcpy(&elf_info->shoff, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shoff)), sizeof(uint64_t) >> !elf_info->is_x64);
}

static void
parse_sym_hdr(t_elf_info* elf_info, t_file* file, Elf64_Sym* sym_hdr, Elf64_Shdr* s_symtab) {
	size_t const	nb_entry = s_symtab->sh_size / s_symtab->sh_entsize;
	uint64_t		offset;
	Elf64_Shdr*		tmp;

	tmp = get_section_hdr_by_index(ptr_add(file->buffer, elf_info->shoff), s_symtab->sh_link);
	offset = tmp->sh_offset;
	for (size_t i = 0; i < nb_entry; ++i) {
		if (((char *)ptr_add(file->buffer, offset + sym_hdr->st_name))[0])
			printf("%s\n", (char *)ptr_add(file->buffer, offset + sym_hdr->st_name));
		sym_hdr = ptr_add(sym_hdr, s_symtab->sh_entsize);
	}
}

static void
parse_section_hdr(t_elf_info* elf_info, t_file* file) {
	Elf64_Shdr*	s_hdr;
	size_t		shdr_off;

	memcpy(&shdr_off, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shoff)), (8 >> !elf_info->is_x64));
	s_hdr = ptr_add(file->buffer, shdr_off);
	for (size_t i = 0; i < elf_info->nb_shdr; ++i) {
		if (s_hdr->sh_type == SHT_SYMTAB)
			parse_sym_hdr(elf_info, file, ptr_add(file->buffer, s_hdr->sh_offset), s_hdr);
		s_hdr = ptr_add(s_hdr, elf_info->size_shdr);
	}
}

static void
parse_elf(t_ctx* ctx, t_file* file, char* file_path) {
	t_elf_info	elf_info = { 0 };

	if (check_elf_hdr(file, ctx, file_path))
		return ;
	if (ctx->nb_binary != 1)
		printf("%s:\n", file_path);
	get_elf_info(&elf_info, file);
	parse_section_hdr(&elf_info, file);
}

void
parse_all_elf(t_ctx* ctx) {
	t_file	file;

	for (size_t i = 0; i < ctx->nb_binary; ++i) {
		map_file(&file, ctx->binary_file_path[i]);
		parse_elf(ctx, &file, ctx->binary_file_path[i]);
		if (i != ctx->nb_binary - 1 && ctx->nb_binary != 1)
			printf("\n");
		munmap(file.buffer, file.size);
	}
}
