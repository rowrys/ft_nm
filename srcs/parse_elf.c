
#include "file.h"
#include "ft_nm.h"
#include "parse_elf.h"
#include "symbols_info.h"
#include "symbols_type.h"
#include "utils.h"

#include <elf.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

static inline void
out_of_range(t_ctx* ctx) {
	dprintf(STDERR_FILENO, MSG_ERROR_OUT_OF_RANGE, ctx->current_binary_file_path);
	munmap(ctx->file->buffer, ctx->file->size);
	destoy_ctx(ctx, EXIT_FAILURE);
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
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid class */
		ctx->exit_code = 1;
		return (1);
	}
	if (!memcmp(ptr_add(ctx->file->buffer, OFFSET_EHDR(is_x64, e_type)), &et_rel, 2)
			&& !memcmp(ptr_add(ctx->file->buffer, OFFSET_EHDR(is_x64, e_type)), &et_dyn, 2)) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG); /* Invalid file format not (.so .o x86(_64)) */
		ctx->exit_code = 1;
		return (1);
	}
	return (0);
}

static void
get_elf_info(t_elf_info* elf_info, t_file* file) {
	elf_info->is_x64 = *((uchar*)ptr_add(file->buffer, EI_CLASS)) == (uchar)ELFCLASS64;
	memcpy(&elf_info->nb_shdr, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shnum)), 2);
	memcpy(&elf_info->size_shdr, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shentsize)), 2);
	memcpy(&elf_info->shstrndx, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shstrndx)), 2);
	memcpy(&elf_info->sh, ptr_add(file->buffer, OFFSET_EHDR(elf_info->is_x64, e_shoff)), sizeof(uint64_t) >> !elf_info->is_x64);
	elf_info->sh = ptr_add(file->buffer, (size_t)elf_info->sh);
}

static uint64_t
get_nb_entry(void* s_hdr, bool is_x64) {
	uint64_t	s_hdr_size;
	uint64_t	s_hdr_entsize;

	shdrcpy_size(&s_hdr_size, s_hdr, is_x64);
	shdrcpy_entsize(&s_hdr_entsize, s_hdr, is_x64);
	return (s_hdr_size / s_hdr_entsize);
}

static char*
get_sym_name(t_ctx* ctx, uint64_t offset, uint32_t sym_hdr_name, void* sym_hdr) {
	void*		symbol_relation_section;		/* Elf64_Shdr/Elf32_Shdr */
	void*		s_strtab;						/* Elf64_Shdr/Elf32_Shdr */
	char*		result;
	uint16_t	st_shndx;
	uint32_t	s_hdr_name;
	uint64_t	strtab_offset;
	uchar		sym_hdr_info;

	s_strtab = get_s_hdr_by_index(ctx->elf_info->sh, ctx->elf_info->shstrndx, ctx->elf_info->is_x64);
	if (check_addr(ctx->file, s_strtab))
		out_of_range(ctx);
	symhdrcpy_info(&sym_hdr_info, sym_hdr, ctx->elf_info->is_x64);
	if (ELF_ST_TYPE(sym_hdr_info) == STT_SECTION) {
		symhdrcpy_shndx(&st_shndx, sym_hdr, ctx->elf_info->is_x64);
		shdrcpy_offset(&strtab_offset, s_strtab, ctx->elf_info->is_x64);
		symbol_relation_section = get_s_hdr_by_index(ctx->elf_info->sh, st_shndx, ctx->elf_info->is_x64);
		if (check_addr(ctx->file, symbol_relation_section))
			out_of_range(ctx);
		shdrcpy_name(&s_hdr_name, symbol_relation_section, ctx->elf_info->is_x64);
		result = ptr_add(ctx->file->buffer, strtab_offset + s_hdr_name);
	}
	else
		result = ptr_add(ctx->file->buffer, offset + sym_hdr_name);
	return (check_addr(ctx->file, result) ? NULL : result);
}

static void
parse_sym_hdr(t_ctx* ctx, void* sym_hdr, void* s_symtab) {
	bool const		is_x64 = ctx->elf_info->is_x64;
	size_t const	nb_entry = get_nb_entry(s_symtab, ctx->elf_info->is_x64);
	uint64_t		offset;
	uint32_t		sym_hdr_name;
	uint64_t		sym_hdr_value;
	uint64_t		s_hdr_entsize;
	uint32_t		s_hdr_link;

	shdrcpy_link(&s_hdr_link, s_symtab, is_x64);
	shdrcpy_offset(&offset, get_s_hdr_by_index(ctx->elf_info->sh, s_hdr_link, is_x64), is_x64);
	shdrcpy_entsize(&s_hdr_entsize, s_symtab, is_x64);
	ctx->symbols_info_len = 0;
	for (size_t i = 0; i < nb_entry; ++i) {
		symhdrcpy_name(&sym_hdr_name, sym_hdr, is_x64);
		if (i && (((char *)ptr_add(ctx->file->buffer, offset + sym_hdr_name))[0] || get_option_stat(ctx->options, OPT_A))) {
			symhdrcpy_value(&sym_hdr_value, sym_hdr, is_x64);
			add_symbol_info(ctx, sym_hdr_value, get_symbos_type(ctx, sym_hdr), get_sym_name(ctx, offset, sym_hdr_name, sym_hdr));
		}
		sym_hdr = ptr_add(sym_hdr, s_hdr_entsize);
		if (!sym_hdr)
			out_of_range(ctx);
	}
}

static void
parse_section_hdr(t_ctx* ctx) {
	void*		s_hdr;			/* Elf64_Shdr / Elf32_Shdr */
	uint32_t	s_hdr_type;
	uint64_t	s_hdr_offset;

	s_hdr = ctx->elf_info->sh;
	for (size_t i = 0; i < ctx->elf_info->nb_shdr; ++i) {
		shdrcpy_type(&s_hdr_type, s_hdr, ctx->elf_info->is_x64);
		if (s_hdr_type == SHT_SYMTAB) {
			shdrcpy_offset(&s_hdr_offset, s_hdr, ctx->elf_info->is_x64);
			init_symbols_info(ctx, ctx->file, s_hdr, ctx->elf_info->is_x64);
			parse_sym_hdr(ctx, ptr_add(ctx->file->buffer, s_hdr_offset), s_hdr);
		}
		s_hdr = next_hdr(s_hdr, ctx->elf_info->size_shdr, ctx->file);
		if (!s_hdr)
			out_of_range(ctx);
	}
}

static void
parse_elf(t_ctx* ctx) {
	t_elf_info	elf_info = { 0 };

	ctx->elf_info = &elf_info;
	if (check_elf_hdr(ctx))
		return ;
	if (ctx->nb_binary != 1)
		printf("%s:\n", ctx->current_binary_file_path);
	get_elf_info(&elf_info, ctx->file);
	if (check_addr(ctx->file, elf_info.sh))
		out_of_range(ctx);
	parse_section_hdr(ctx);
}

void
parse_all_elf(t_ctx* ctx) {
	t_file	file;

	for (size_t i = 0; i < ctx->nb_binary; ++i) {
		ctx->current_binary_file_path = ctx->binary_file_path[i];
		map_file(&file, ctx->current_binary_file_path);
		ctx->file = &file;
		parse_elf(ctx);
		display_symbols_info(ctx);
		if (i != ctx->nb_binary - 1 && ctx->nb_binary != 1)
			printf("\n");
		munmap(file.buffer, file.size);
	}
}
