#ifndef ELF_POSTFIX
# error ELF_POSTFIX is not defined
#endif
#ifndef ELF_SHDR
# error ELF_SHDR is not defined
#endif
#ifndef ELF_SYM
# error ELF_SYM is not defined
#endif

#define ELF_GLUE_I(x, y) x##y
#define ELF_GLUE(x, y) ELF_GLUE_I(x, y)
#define ELF_NAMED(x) ELF_GLUE(x, ELF_POSTFIX)

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static void
ELF_NAMED(parse_sym_hdr)(t_ctx* ctx, ELF_SYM* sym_hdr, ELF_SHDR* s_symtab) {
	bool const		is_x64 = ctx->elf_info->is_x64;
	size_t const	nb_entry = get_nb_entry(s_symtab, ctx->elf_info->is_x64);
	uint64_t		offset;

	offset = ((ELF_SHDR *)(get_s_hdr_by_index(ctx->elf_info->sh, s_symtab->sh_link, is_x64)))->sh_offset;
	ctx->symbols_info_len = 0;
	for (size_t i = 0; i < nb_entry; ++i) {
		if (i && (((char *)ptr_add(ctx->file->buffer, offset + sym_hdr->st_name))[0] || get_option_stat(ctx->options, OPT_A))) {
			add_symbol_info(ctx, sym_hdr->st_value, get_symbos_type(ctx, sym_hdr), get_sym_name(ctx, offset, sym_hdr->st_name, sym_hdr));
		}
		sym_hdr = ptr_add(sym_hdr, s_symtab->sh_entsize);
		if (!sym_hdr)
			out_of_range(ctx);
	}
}


static void
ELF_NAMED(parse_section_hdr)(t_ctx* ctx) {
	ELF_SHDR*	s_hdr;

	s_hdr = ctx->elf_info->sh;
	for (size_t i = 0; i < ctx->elf_info->nb_shdr; ++i) {
		if (s_hdr->sh_type == SHT_SYMTAB) {
			init_symbols_info(ctx, ctx->file, s_hdr, ctx->elf_info->is_x64);
			ELF_NAMED(parse_sym_hdr)(ctx, ptr_add(ctx->file->buffer, s_hdr->sh_offset), s_hdr);
		}
		s_hdr = next_hdr(s_hdr, ctx->elf_info->size_shdr, ctx->file);
		if (!s_hdr)
			out_of_range(ctx);
	}
}

#undef ELF_GLUE_I
#undef ELF_GLUE
#undef ELF_NAMED

#undef ELF_POSTFIX
#undef ELF_SHDR
#undef ELF_SYM
