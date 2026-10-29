#include "ft_nm.h"
#include "arguments.h"
#include "parse_elf.h"

#include <stdlib.h>

int
main(int argc, char** argv) {
	t_ctx	ctx = { 0 };

	(void)argc;
	parse_argument(&ctx, argv);
	parse_all_elf(&ctx);
	free(ctx.binary_file_path);
	return (ctx.exit_code);
}
