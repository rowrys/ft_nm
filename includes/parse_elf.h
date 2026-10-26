#ifndef PARSE_ELF_H
# define PARSE_ELF_H

# include "ft_nm.h"
# include "file.h"

# include <stdbool.h>
# include <string.h>
# include <elf.h>

static inline bool
is_elf(t_file* file) {
	if (file->size < SELFMAG)
		return (0);
	return (!memcmp(file->buffer, ELFMAG, SELFMAG));
}

void	parse_all_elf(t_ctx* ctx);

#endif
