
#include "ft_nm.h"
#include "file.h"
#include "arguments.h"

#include <elf.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>

void
destoy_ctx(t_ctx* ctx)  {
	free(ctx->binary_file_path);
}

void
parse_elf(t_ctx* ctx) {
	t_file	file;
	
	for (size_t i = 0; i < ctx->nb_binary; ++i) {
		map_file(&file, ctx->binary_file_path[i]);
		
		munmap(file.buffer, file.size);
	}
}

int
main(int argc, char** argv) {
	t_ctx	ctx = { 0 };

	(void)argc;
	parse_argument(&ctx, argv);

	parse_elf(&ctx);

	destoy_ctx(&ctx);
	return (0);
}
