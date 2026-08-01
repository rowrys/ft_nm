 
#include "symbols_type.h"
#include "file.h"
#include "utils.h"
#include "parse_elf.h"

// static char*
// get_name_of_s_hdr(void* s_hdr, t_file* file, t_elf_info* elf_info) {
// 	void*		s_hdr_strtab;
// 	uint32_t	s_hdr_name;
// 	uint64_t	s_hdr_strtab_offset;

// 	shdrcpy_name(&s_hdr_name, s_hdr, elf_info->is_x64);
// 	s_hdr_strtab = get_s_hdr_by_index(ptr_add(file->buffer, elf_info->shoff), elf_info->shstrndx, elf_info->is_x64);
// 	shdrcpy_offset(&s_hdr_strtab_offset, s_hdr_strtab, elf_info->is_x64);
// 	return (ptr_add(file->buffer, s_hdr_strtab_offset + s_hdr_name));
// }

static char
get_type_from_progbits(t_elf_info* elf_info, void* symbol_relation_section) {
	uint64_t	s_hdr_flags;

	shdrcpy_flags(&s_hdr_flags, symbol_relation_section, elf_info->is_x64);
	if (s_hdr_flags & SHF_ALLOC && s_hdr_flags & SHF_WRITE)
		return ('d');
	else if (s_hdr_flags & SHF_ALLOC && s_hdr_flags & SHF_EXECINSTR)
		return ('t');
	else if (s_hdr_flags == SHF_ALLOC)
		return ('r');
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

char
get_symbos_type(t_elf_info* elf_info, t_file* file, void* sym_hdr) {
	void*		symbol_relation_section;		/* Elf64_Shdr/Elf32_Shdr */
	bool		is_maj;
	uint32_t	s_hdr_type;
	uint32_t	s_hdr_name;
	uint16_t	sym_hdr_shndx;
	uchar		sym_hdr_info;

	symhdrcpy_info(&sym_hdr_info, sym_hdr, elf_info->is_x64);
	symhdrcpy_shndx(&sym_hdr_shndx, sym_hdr, elf_info->is_x64);
	is_maj = (ELF32_ST_BIND(sym_hdr_info) & STB_GLOBAL);
	if (sym_hdr_shndx >= elf_info->nb_shdr)
		return ('*');
	symbol_relation_section = get_s_hdr_by_index(ptr_add(file->buffer, elf_info->shoff), sym_hdr_shndx, elf_info->is_x64);
	shdrcpy_type(&s_hdr_type, symbol_relation_section, elf_info->is_x64);
	shdrcpy_name(&s_hdr_name, symbol_relation_section,  elf_info->is_x64);
	if (ELF_ST_BIND(sym_hdr_info) == STB_WEAK)
		return (get_weak_symbols(sym_hdr_shndx, sym_hdr_info));
	switch (s_hdr_type) {
		case SHT_NOBITS:
			return (convert_gloal_symbol('b', is_maj));
		case SHT_PROGBITS: case SHT_NOTE: case SHT_DYNAMIC: case SHT_INIT_ARRAY: case SHT_FINI_ARRAY:
			return (convert_gloal_symbol(get_type_from_progbits(elf_info, symbol_relation_section), is_maj));
	}
	return (convert_gloal_symbol('u', is_maj));
}
