#ifndef PARSE_ELF_H
# define PARSE_ELF_H

# include "ft_nm.h"
# include "file.h"

# include <stdbool.h>
# include <string.h>
# include <elf.h>

typedef struct s_elf_info {
	void*		sh;				/* addrs of section headers.	*/
	uint16_t	nb_shdr;		/* nombers of section headers	*/
	uint16_t	size_shdr;		/* size of section headers		*/
	uint16_t	shstrndx;		/* index of strtab in sh		*/
	bool		is_x64;

} t_elf_info;

static inline bool
is_elf(t_file* file) {
	if (file->size < SELFMAG)
		return (0);
	return (!memcmp(file->buffer, ELFMAG, SELFMAG));
}

void	parse_all_elf(t_ctx* ctx);

#endif
