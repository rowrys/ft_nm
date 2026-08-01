#ifndef UTILS_H
# define UTILS_H

# include "ft_nm.h"

# include <elf.h>
# include <stdint.h>
# include <stdbool.h>
# include <string.h>

# define OFFSET_EHDR(is_x64, attribute) (is_x64 == 1 ? offsetof(Elf64_Ehdr, attribute) : offsetof(Elf32_Ehdr, attribute))
# define OFFSET_SHDR(is_x64, attribute) (is_x64 == 1 ? offsetof(Elf64_Shdr, attribute) : offsetof(Elf32_Shdr, attribute))
# define OFFSET_PHDR(is_x64, attribute) (is_x64 == 1 ? offsetof(Elf64_Phdr, attribute) : offsetof(Elf32_Phdr, attribute))
# define OFFSET_SYM(is_x64, attribute)  (is_x64 == 1 ? offsetof(Elf64_Sym, attribute)  : offsetof(Elf32_Sym, attribute))

# define ELF_ST_BIND(val)			ELF32_ST_BIND(val)
# define ELF_ST_TYPE(val)			ELF32_ST_TYPE(val)
# define ELF_ST_INFO(bind, type)	ELF32_ST_INFO(bind, type)

/**
 * @brief Elf_Shdr headers utils.
 */

static inline void
shdrcpy_type(uint32_t* type, void* s_hdr, bool is_x64) {
	memcpy(type, ptr_add(s_hdr, OFFSET_SHDR(is_x64, sh_type)), sizeof(uint32_t));
}

static inline void
shdrcpy_offset(uint64_t* offset, void* s_hdr, bool is_x64) {
	*offset = 0;
	memcpy(offset, ptr_add(s_hdr, OFFSET_SHDR(is_x64, sh_offset)), sizeof(uint64_t) >> !is_x64);
}

static inline void
shdrcpy_size(uint64_t* s_hdr_size, void* s_hdr, bool is_x64) {
	*s_hdr_size = 0;
	memcpy(s_hdr_size, ptr_add(s_hdr, OFFSET_SHDR(is_x64, sh_size)), sizeof(uint64_t) >> !is_x64);
}

static inline void
shdrcpy_entsize(uint64_t* s_hdr_entsize, void* s_hdr, bool is_x64) {
	*s_hdr_entsize = 0;
	memcpy(s_hdr_entsize, ptr_add(s_hdr, OFFSET_SHDR(is_x64, sh_entsize)), sizeof(uint64_t) >> !is_x64);
}

static inline void
shdrcpy_link(uint32_t* s_hdr_link, void* s_hdr, bool is_x64) {
	memcpy(s_hdr_link, ptr_add(s_hdr, OFFSET_SHDR(is_x64, sh_link)), sizeof(uint32_t));
}

static inline void
shdrcpy_name(uint32_t* s_hdr_name, void* s_hdr, bool is_x64) {
	memcpy(s_hdr_name, ptr_add(s_hdr, OFFSET_SHDR(is_x64, sh_name)), sizeof(uint32_t));
}

static inline void
shdrcpy_flags(uint64_t* s_hdr_flags, void* s_hdr, bool is_x64) {
	*s_hdr_flags = 0;
	memcpy(s_hdr_flags, ptr_add(s_hdr, OFFSET_SHDR(is_x64, sh_flags)), sizeof(uint32_t));
}

/**
 * @brief Elf_Sym headers utils.
 */

static inline void
symhdrcpy_name(uint32_t* name, void* sym_hdr, bool is_x64) {
	memcpy(name, ptr_add(sym_hdr, OFFSET_SYM(is_x64, st_name)), sizeof(uint32_t));
}

static inline void
symhdrcpy_value(uint64_t* value, void* sym_hdr, bool is_x64) {
	*value = 0;
	memcpy(value, ptr_add(sym_hdr, OFFSET_SYM(is_x64, st_value)), sizeof(uint64_t) >> !is_x64);
}

static inline void
symhdrcpy_shndx(uint16_t* sym_hdr_shndx, void* sym_hdr, bool is_x64) {
	memcpy(sym_hdr_shndx, ptr_add(sym_hdr, OFFSET_SYM(is_x64, st_shndx)), sizeof(uint16_t));
}

static inline void
symhdrcpy_info(uchar* sym_hdr_info, void* sym_hdr, bool is_x64) {
	memcpy(sym_hdr_info, ptr_add(sym_hdr, OFFSET_SYM(is_x64, st_info)), sizeof(uchar));
}

#endif
