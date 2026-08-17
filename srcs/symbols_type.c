 
#include "symbols_type.h"
#include "file.h"
#include "ft_nm.h"
#include "utils.h"
#include "parse_elf.h"
#include <stdint.h>

static char
get_type_from_progbits(t_elf_info* elf_info, void* symbol_relation_section) {
	uint64_t	s_hdr_flags;

	shdrcpy_flags(&s_hdr_flags, symbol_relation_section, elf_info->is_x64);
	if (s_hdr_flags & SHF_ALLOC && s_hdr_flags & SHF_WRITE)
		return ('d');
	else if (s_hdr_flags & SHF_ALLOC && s_hdr_flags & SHF_EXECINSTR)
		return ('t');
	else if (s_hdr_flags & SHF_ALLOC)
		return ('r');
	return ('?');
}

static char
get_weak_symbols(uint16_t sym_hdr_shndx, uchar sym_hdr_info) {
	switch (ELF_ST_TYPE(sym_hdr_info)) {
		case STT_FUNC: case STT_NOTYPE:
			return ('w' - (32 * (sym_hdr_shndx != SHN_UNDEF)));
		case STT_OBJECT:
			return ('v' - (32 * (sym_hdr_shndx != SHN_UNDEF)));
	}
	return ('?');
}

static char
get_gnu_symbols(uint16_t sym_hdr_shndx) {
	switch (sym_hdr_shndx) {
		case SHN_ABS:
			return ('A');
		case SHN_COMMON:
			return ('C');
	}
	return ('?');
}

static char
get_specifique_symbol_type(t_ctx* ctx, t_elf_info* elf_info, uint16_t sym_hdr_shndx, uchar sym_hdr_info) {
	if (sym_hdr_info == STT_FILE) {
		if (get_option_stat(ctx->options, OPT_A))
			return ('a');
		return (SYMBOL_TYPE_NON_PRINTABLE);
	}
	if (sym_hdr_shndx > elf_info->nb_shdr)
		return (get_gnu_symbols(sym_hdr_shndx));
	if (ELF_ST_BIND(sym_hdr_info) == STB_WEAK)
		return (get_weak_symbols(sym_hdr_shndx, sym_hdr_info));
	if (ELF_ST_TYPE(sym_hdr_info) == STT_GNU_IFUNC)
		return ('i');
	return (0);
}

static char
get_symbol_type_from_s_hdr(void* symbol_relation_section, uint32_t s_hdr_type, t_elf_info* elf_info, bool is_maj) {
	switch (s_hdr_type) {
		case SHT_NOBITS:
			return (convert_gloal_symbol('b', is_maj));
		case SHT_PROGBITS: case SHT_NOTE: case SHT_DYNAMIC: case SHT_INIT_ARRAY: case SHT_FINI_ARRAY:
			return (convert_gloal_symbol(get_type_from_progbits(elf_info, symbol_relation_section), is_maj));
	}
	return (convert_gloal_symbol('u', is_maj));
}

char
get_symbos_type(t_ctx* ctx, t_elf_info* elf_info, void* sym_hdr) {
	void*		symbol_relation_section;		/* Elf64_Shdr/Elf32_Shdr */
	bool		is_maj;
	uint32_t	s_hdr_type;
	uint32_t	s_hdr_name;
	uint16_t	sym_hdr_shndx;
	uchar		sym_hdr_info;
	char		result;

	symhdrcpy_info(&sym_hdr_info, sym_hdr, elf_info->is_x64);
	symhdrcpy_shndx(&sym_hdr_shndx, sym_hdr, elf_info->is_x64);
	is_maj = (ELF32_ST_BIND(sym_hdr_info) & STB_GLOBAL);
	result = get_specifique_symbol_type(ctx, elf_info, sym_hdr_shndx, sym_hdr_info);
	if (result)
		return (result);
	symbol_relation_section = get_s_hdr_by_index(elf_info->sh, sym_hdr_shndx, elf_info->is_x64);
	shdrcpy_type(&s_hdr_type, symbol_relation_section, elf_info->is_x64);
	shdrcpy_name(&s_hdr_name, symbol_relation_section,  elf_info->is_x64);
	return (get_symbol_type_from_s_hdr(symbol_relation_section, s_hdr_type, elf_info, is_maj));
}
