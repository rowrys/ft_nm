#ifndef FT_NM_H
# define FT_NM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

# define DEFAULT_FILE_PATH "a.out"
# define DEFAULT_ERROR_MSG "nm: FATAL\n"
# define MSG_ERROR_INVALID_OPTION "Usage: nm [option(s)] [file(s)]\n List symbols in [file(s)] (a.out by default).\n The options are:\n  -a, --debug-syms       Display debugger-only symbols\n  -g, --extern-only      Display only external symbols\n    --ifunc-chars=CHARS  Characters to use when displaying ifunc symbols\n  -p, --no-sort          Do not sort the symbols\n  -r, --reverse-sort     Reverse the sense of the sort\n      --plugin NAME      Load the specified plugin\n  -u, --undefined-only   Display only undefined symbols\n"

typedef unsigned char uchar;

typedef struct s_ctx {
	size_t	nb_binary;				/* numbers of binary to analyse.*/
	char**	binary_file_path;		/* file path of the binary.		*/
	size_t	options;				/* store the differents option	*/
	int		exit_code;				/* exit code of ft_nm			*/
}	t_ctx;

typedef enum e_nm_options {
	OPT_A = 1,
	OPT_G = 2,
	OPT_U = 4,
	OPT_R = 8,
	OPT_P = 16,
}	t_nm_options;

static inline void
set_option_stat(size_t* options, uint8_t option_pos, bool state) {
	*options = (state) ? *options | option_pos : *options & ~option_pos;
}

# define OFFSET_EHDR(is_x64, attribute) (offsetof(Elf64_Ehdr, attribute) * is_x64) + (offsetof(Elf32_Ehdr, attribute) * !is_x64)
# define OFFSET_SHDR(is_x64, attribute) (offsetof(Elf64_Shdr, attribute) * is_x64) + (offsetof(Elf32_Shdr, attribute) * !is_x64)
# define OFFSET_PHDR(is_x64, attribute) (offsetof(Elf64_Phdr, attribute) * is_x64) + (offsetof(Elf32_Phdr, attribute) * !is_x64)

#endif
