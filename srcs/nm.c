
#include "ft_nm.h"
#include "arguments.h"
#include "parse_elf.h"

#include <stdlib.h>

void
destoy_ctx(t_ctx* ctx)  {
	free(ctx->binary_file_path);
	free(ctx->symbols_info);
}

int
main(int argc, char** argv) {
	t_ctx	ctx = { 0 };

	(void)argc;
	parse_argument(&ctx, argv);
	parse_all_elf(&ctx);
	destoy_ctx(&ctx);
	return (ctx.exit_code);
}
