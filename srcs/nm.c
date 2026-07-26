
#include "ft_nm.h"
#include "file.h"
#include "arguments.h"

#include <elf.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void
destoy_ctx(t_ctx* ctx)  {
	free(ctx->binary_file_path);
}

int
main(int argc, char** argv) {
	t_ctx	ctx = { 0 };
	uchar*	file;
	char*	file_path;

	(void)argc;
	file_path = DEFAULT_FILE_PATH;
	parse_argument(&ctx, argv);
	for (size_t i = 0; i < ctx.nb_binary; ++i) {
		printf("%s\n", ctx.binary_file_path[i]);
	}
 	file = map_file(file_path);
	destoy_ctx(&ctx);
	(void)file;
	return (0);
}
