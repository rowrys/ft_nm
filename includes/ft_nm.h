#ifndef FT_NM_H
# define FT_NM_H

# include "symbols_info.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <elf.h>

# define DEFAULT_FILE_PATH "a.out"
# define DEFAULT_ERROR_MSG "nm: FATAL\n"
# define MSG_ERROR_INVALID_OPTION "Usage: nm [option(s)] [file(s)]\n List symbols in [file(s)] (a.out by default).\n The options are:\n  -a, --debug-syms       Display debugger-only symbols\n  -g, --extern-only      Display only external symbols\n    --ifunc-chars=CHARS  Characters to use when displaying ifunc symbols\n  -p, --no-sort          Do not sort the symbols\n  -r, --reverse-sort     Reverse the sense of the sort\n      --plugin NAME      Load the specified plugin\n  -u, --undefined-only   Display only undefined symbols\n"

typedef unsigned char uchar;

typedef struct s_ctx {
	size_t			nb_binary;				/* numbers of binary to analyse.*/
	char**			binary_file_path;		/* file path of the binary.		*/
	size_t			options;				/* store the differents option	*/
	t_symbol_info*	symbols_info;			/* tab of symbol_info			*/
	size_t			symbols_info_len;		/* lenght of symbols_info		*/
	int				exit_code;				/* exit code of ft_nm			*/
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

static inline void*
ptr_add(void* ptr, size_t n) {
	return ((uchar*)ptr + n);
}

static inline void*
ptr_sub(void* ptr, size_t n) {
	return ((uchar*)ptr - n);
}

static inline void*
get_s_hdr_by_index(void* section_hdr_tab, uint32_t idx, bool is_x64) {
	return (is_x64 == 1 ? ptr_add(section_hdr_tab, sizeof(Elf64_Shdr) * idx) : ptr_add(section_hdr_tab, sizeof(Elf32_Shdr) * idx));
}

#endif
