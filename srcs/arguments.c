
#include "ft_nm.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

static void
get_options(t_ctx* ctx, char* opt) {
	for (size_t i = 1; opt[i]; ++i) {
		switch (opt[i]) {
			case 'a':
				set_option_stat(&(ctx->options), OPT_A, 1);
				continue ;
			case 'g':
				set_option_stat(&(ctx->options), OPT_G, 1);
				continue ;
			case 'p':
				set_option_stat(&(ctx->options), OPT_P, 1);
				continue ;
			case 'r':
				set_option_stat(&(ctx->options), OPT_R, 1);
				continue ;
			case 'u':
				set_option_stat(&(ctx->options), OPT_U, 1);
				continue ;
			case '-':
				dprintf(STDERR_FILENO, "nm: unrecognized option '%s'\n", opt);
				dprintf(STDERR_FILENO, MSG_ERROR_INVALID_OPTION);
				exit(1);
			default:
				dprintf(STDERR_FILENO, "nm: option requires an argument -- '%c'\n", opt[i]);
				dprintf(STDERR_FILENO, MSG_ERROR_INVALID_OPTION);
				exit(1);
		}
	}
}

static void
set_default_binary_path(t_ctx* ctx) {
	ctx->binary_file_path = malloc(sizeof(char*));
	if (!ctx->binary_file_path) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		exit(1);
	}
	ctx->binary_file_path[0] = DEFAULT_FILE_PATH;
	++ctx->nb_binary;
}

static void
set_binarys_path(t_ctx* ctx, char** argv) {
	size_t	idx;

	ctx->binary_file_path = malloc(sizeof(char*) * ctx->nb_binary);
	if (!ctx->binary_file_path) {
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		exit(1);
	}
	idx = 0;
	for (size_t i = 1; argv[i]; ++i) {
		if (argv[i][0] == '-' && argv[i][1])
			continue ;
		else
			ctx->binary_file_path[idx++] = argv[i];
	}
}

static void
set_flags_priority(size_t* options) {
	if (get_option_stat(*options, OPT_U))
		*options = *options & ~(OPT_A | OPT_G);
	else if (get_option_stat(*options, OPT_G))
		*options = *options & ~OPT_A;
}

void
parse_argument(t_ctx* ctx, char** argv) {
	for (size_t i = 1; argv[i]; ++i) {
		if (argv[i][0] == '-' && argv[i][1])
			get_options(ctx, argv[i]);
		else
			++ctx->nb_binary;
	}
	if (ctx->nb_binary)
		set_binarys_path(ctx, argv);
	else
		set_default_binary_path(ctx);
	set_flags_priority(&ctx->options);
}
