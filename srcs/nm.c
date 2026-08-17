
#include "ft_nm.h"
#include "arguments.h"
#include "parse_elf.h"

#include <stdint.h>
#include <stdlib.h>

void
destoy_ctx(t_ctx* ctx, uint8_t exit_code)  {
	free(ctx->binary_file_path);
	free(ctx->symbols_info);
	exit(exit_code);
}

int
main(int argc, char** argv) {
	t_ctx	ctx = { 0 };

	(void)argc;
	parse_argument(&ctx, argv);
	parse_all_elf(&ctx);
	destoy_ctx(&ctx, ctx.exit_code);
}
