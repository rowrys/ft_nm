
#include "file.h"
#include "parse_elf.h"

#include <sys/mman.h>
#include <unistd.h>

static bool
parse_elf(t_ctx* ctx, t_file* file, char* file_path) {
	if (!is_elf(file)) {
		dprintf(STDERR_FILENO, "nm: %s: file format not recognized\n", file_path);
		ctx->exit_code = 1;
		return (1);
	}
	printf("%s:\n", file_path);
	return (0);
}

void
parse_all_elf(t_ctx* ctx) {
	t_file	file;

	for (size_t i = 0; i < ctx->nb_binary; ++i) {
		map_file(&file, ctx->binary_file_path[i]);
		parse_elf(ctx, &file, ctx->binary_file_path[i]);
		munmap(file.buffer, file.size);
	}
}
