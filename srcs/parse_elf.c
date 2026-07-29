
#include "file.h"
#include "ft_nm.h"
#include "parse_elf.h"
#include "utils.h"

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

// static char
// get_symbos_type(t_elf_info* elf_info, t_file* file, Elf64_Sym* sym_hdr) {
// 	Elf64_Shdr*	symbol_relation_section;

// 	(void)elf_info;
// 	symbol_relation_section = ptr_add(file->buffer, sym_hdr->st_shndx);
// 	printf("type{%d}", sym_hdr->st_shndx);
// 	switch (symbol_relation_section->sh_type) {
// 		case SHT_NOBITS:
// 			return ('b');
// 	}
// 	if (ELF32_ST_BIND(sym_hdr->st_info) == STB_WEAK)
// 		return ('w');
// 	return ('?');
// }

static uint64_t
get_nb_entry(void* s_hdr, bool is_x64) {
	uint64_t	s_hdr_size;
	uint64_t	s_hdr_entsize;

	shdrcpy_size(&s_hdr_size, s_hdr, is_x64);
	shdrcpy_entsize(&s_hdr_entsize, s_hdr, is_x64);
	return (s_hdr_size / s_hdr_entsize);
}

static void
parse_sym_hdr(t_elf_info* elf_info, t_file* file, void* sym_hdr, void* s_symtab) {
	size_t const	nb_entry = get_nb_entry(s_symtab, elf_info->is_x64);
	uint64_t		offset;
	uint32_t		sym_hdr_name;
	uint64_t		sym_hdr_value;
	uint64_t		s_hdr_entsize;
	uint32_t		s_hdr_link;

	shdrcpy_link(&s_hdr_link, s_symtab, elf_info->is_x64);
	shdrcpy_offset(&offset, get_s_hdr_by_index(ptr_add(file->buffer, elf_info->shoff), s_hdr_link, elf_info->is_x64), elf_info->is_x64);
	shdrcpy_entsize(&s_hdr_entsize, s_symtab, elf_info->is_x64);
	for (size_t i = 0; i < nb_entry; ++i) {
		symhdrcpy_name(&sym_hdr_name, sym_hdr, elf_info->is_x64);
		if (((char *)ptr_add(file->buffer, offset + sym_hdr_name))[0]) {
			symhdrcpy_value(&sym_hdr_value, sym_hdr, elf_info->is_x64);
			printf("%.16zx ", sym_hdr_value);
			// printf("%c ", get_symbos_type(elf_info, file, sym_hdr));
			printf("%s\n", (char *)ptr_add(file->buffer, offset + sym_hdr_name));
		}
		sym_hdr = ptr_add(sym_hdr, s_hdr_entsize);
	}
}

static void
parse_section_hdr(t_elf_info* elf_info, t_file* file) {
	void*		s_hdr;			/* Elf64_Shdr / Elf32_Shdr */
	uint32_t	s_hdr_type;
	uint64_t	s_hdr_offset;

	s_hdr = ptr_add(file->buffer, elf_info->shoff);
	for (size_t i = 0; i < elf_info->nb_shdr; ++i) {
		shdrcpy_type(&s_hdr_type, s_hdr, elf_info->is_x64);
		if (s_hdr_type == SHT_SYMTAB) {
			shdrcpy_offset(&s_hdr_offset, s_hdr, elf_info->is_x64);
			parse_sym_hdr(elf_info, file, ptr_add(file->buffer, s_hdr_offset), s_hdr);
		}
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
