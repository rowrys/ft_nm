#include <stdlib.h>
#include <sys/mman.h>

static inline void
ELF_NAMED(init_symbols_info)(t_ctx* ctx, t_file* file, ELF_SHDR* s_hdr) {
	ctx->symbols_info_len = s_hdr->sh_size / s_hdr->sh_entsize;
	ctx->symbols_info = malloc(sizeof(t_symbol_info) * ctx->symbols_info_len);
	if (!ctx->symbols_info) {
		munmap(file->buffer, file->size);
		free(ctx->binary_file_path);
		dprintf(STDERR_FILENO, ERROR_MSG_MEM_ALLOC);
		exit(1);
	}
}
