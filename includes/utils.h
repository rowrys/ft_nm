#ifndef UTILS_H
# define UTILS_H

# include "ft_nm.h"

# include <elf.h>
# include <stdint.h>
# include <stdbool.h>
# include <string.h>

# define OFFSET_EHDR(is_x64, attribute) (is_x64 ? offsetof(Elf64_Ehdr, attribute) : offsetof(Elf32_Ehdr, attribute))

# define ELF_ST_BIND(val)			ELF32_ST_BIND(val)
# define ELF_ST_TYPE(val)			ELF32_ST_TYPE(val)
# define ELF_ST_INFO(bind, type)	ELF32_ST_INFO(bind, type)

/**
 * @brief this function check if the offset is out of range.
 * 
 * @param file_size the total size of the file
 * @param offset offset to check
 * @return true invalid offset (out of range)
 * @return false valid offset
 */
static inline bool
check_offset(size_t file_size, size_t offset) {
	return (offset > file_size);
}

/**
 * @brief this function check if an addres is out of range.
 * 
 * @param file file addres
 * @param addr addres to check
 * @return true invalid addr (out of range)
 * @return false valid addr
 */
static inline bool
check_addr(t_file* file, void* addr) {
	if (file->buffer > addr)
		return (1);
	return (check_offset(file->size, (size_t)ptr_sub(addr, (size_t)file->buffer)));
}

static inline void*
next_hdr(void* s_hdr, uint16_t size_shdr, t_file* file) {
	void*	final_addr;

	final_addr = ptr_add(s_hdr, size_shdr);
	if (check_addr(file, ptr_add(s_hdr, size_shdr)))
		return (NULL);
	return (final_addr);
}

#endif
