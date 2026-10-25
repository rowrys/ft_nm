#ifndef ELF_POSTFIX
# error ELF_POSTFIX is not defined
#endif
#ifndef ELF_SHDR
# error ELF_SHDR is not defined
#endif
#ifndef ELF_SYM
# error ELF_SYM is not defined
#endif
#ifndef ELF_EHDR
# error Elf_EHDR is not defined
#endif

#define ELF_GLUE_I(x, y) x##y
#define ELF_GLUE(x, y) ELF_GLUE_I(x, y)
#define ELF_NAMED(x) ELF_GLUE(x, ELF_POSTFIX)

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ELF_INFO ((ELF_EHDR*)(ctx->file->buffer))

#ifndef ELF_SYMBOLS_INFO

#include <stdlib.h>
#include <sys/mman.h>

static inline void
ELF_NAMED(init_symbols_info)(t_ctx* ctx, t_file* file, ELF_SHDR* s_hdr) {
	ctx->symbols_info_len = s_hdr->sh_size / s_hdr->sh_entsize;
	ctx->symbols_info = malloc(sizeof(t_symbol_info) * ctx->symbols_info_len);
	if (!ctx->symbols_info) {
		munmap(file->buffer, file->size);
		free(ctx->binary_file_path);
		dprintf(STDERR_FILENO, DEFAULT_ERROR_MSG);
		exit(1);
	}
}

#endif
#ifdef ELF_SYMBOLS

static char
ELF_NAMED(get_type_from_progbits)(ELF_SHDR* symbol_relation_section) {
	uint64_t const	s_hdr_flags = symbol_relation_section->sh_flags;

	if (s_hdr_flags & SHF_ALLOC && s_hdr_flags & SHF_WRITE)
		return ('d');
	else if (s_hdr_flags & SHF_ALLOC && s_hdr_flags & SHF_EXECINSTR)
		return ('t');
	else if (s_hdr_flags & SHF_ALLOC)
		return ('r');
	return ('?');
}

static char
ELF_NAMED(get_symbol_type_from_s_hdr)(ELF_SHDR* symbol_relation_section, uint32_t s_hdr_type, bool is_maj) {
	switch (s_hdr_type) {
		case SHT_NOBITS:
			return (convert_gloal_symbol('b', is_maj));
		case SHT_PROGBITS: case SHT_NOTE: case SHT_DYNAMIC: case SHT_INIT_ARRAY: case SHT_FINI_ARRAY:
			return (convert_gloal_symbol(ELF_NAMED(get_type_from_progbits)(symbol_relation_section), is_maj));
	}
	return (convert_gloal_symbol('u', is_maj));
}

char
ELF_NAMED(get_symbos_type)(t_ctx* ctx, ELF_SYM* sym_hdr) {
	ELF_SHDR*	symbol_relation_section;
	bool		is_maj;
	char		result;

	is_maj = (ELF32_ST_BIND(sym_hdr->st_info) & STB_GLOBAL);
	result = get_specifique_symbol_type(ctx, ELF_INFO->e_shnum, sym_hdr->st_shndx, sym_hdr->st_info);
	if (result)
		return (result);
	symbol_relation_section = (ELF_SHDR*)ptr_add(ctx->file->buffer, ELF_INFO->e_shoff) + sym_hdr->st_shndx;
	return (ELF_NAMED(get_symbol_type_from_s_hdr)(symbol_relation_section, symbol_relation_section->sh_type, is_maj));
}

#endif
#ifdef ELF_PARSE

char	ELF_NAMED(get_symbos_type)(t_ctx* ctx, ELF_SYM* sym_hdr);
void	ELF_NAMED(init_symbols_info)(t_ctx* ctx, t_file* file, ELF_SHDR* s_hdr);

static inline uint64_t
ELF_NAMED(get_nb_entry)(ELF_SHDR* s_hdr) {
	return (s_hdr->sh_size / s_hdr->sh_entsize);
}

static inline char*
ELF_NAMED(get_sym_name)(t_ctx* ctx, uint64_t offset, uint32_t sym_hdr_name, ELF_SYM* sym_hdr) {
	ELF_SHDR*	symbol_relation_section;
	ELF_SHDR*	s_strtab;
	char*		result;

	s_strtab = (ELF_SHDR*)ptr_add(ctx->file->buffer, ELF_INFO->e_shoff) + ELF_INFO->e_shstrndx;
	if (check_addr(ctx->file, s_strtab))
		out_of_range(ctx);
	if (ELF_ST_TYPE(sym_hdr->st_info) == STT_SECTION) {
		symbol_relation_section = (ELF_SHDR*)ptr_add(ctx->file->buffer, ELF_INFO->e_shoff) + sym_hdr->st_shndx;
		if (check_addr(ctx->file, symbol_relation_section))
			out_of_range(ctx);
		result = ptr_add(ctx->file->buffer, s_strtab->sh_offset + symbol_relation_section->sh_name);
	}
	else
		result = ptr_add(ctx->file->buffer, offset + sym_hdr_name);
	return (check_addr(ctx->file, result) ? NULL : result);
}

static inline void
ELF_NAMED(parse_sym_hdr)(t_ctx* ctx, ELF_SYM* sym_hdr, ELF_SHDR* s_symtab) {
	size_t const	nb_entry = ELF_NAMED(get_nb_entry)(s_symtab);
	uint64_t		offset;

	offset = ((ELF_SHDR *)((ELF_SHDR*)ptr_add(ctx->file->buffer, ELF_INFO->e_shoff) + s_symtab->sh_link))->sh_offset;
	ctx->symbols_info_len = 0;
	for (size_t i = 0; i < nb_entry; ++i) {
		if (i && (((char *)ptr_add(ctx->file->buffer, offset + sym_hdr->st_name))[0] || get_option_stat(ctx->options, OPT_A))) {
			add_symbol_info(ctx, sym_hdr->st_value, ELF_NAMED(get_symbos_type)(ctx, sym_hdr), ELF_NAMED(get_sym_name)(ctx, offset, sym_hdr->st_name, sym_hdr));
		}
		sym_hdr = ptr_add(sym_hdr, s_symtab->sh_entsize);
		if (!sym_hdr)
			out_of_range(ctx);
	}
}


static inline void
ELF_NAMED(parse_section_hdr)(t_ctx* ctx) {
	ELF_SHDR*	s_hdr;

	if (check_addr(ctx->file, ptr_add(ctx->file->buffer, ELF_INFO->e_shoff)))
		out_of_range(ctx);
	s_hdr = ptr_add(ctx->file->buffer, ELF_INFO->e_shoff);
	for (size_t i = 0; i < ELF_INFO->e_shnum; ++i) {
		if (s_hdr->sh_type == SHT_SYMTAB) {
			ELF_NAMED(init_symbols_info)(ctx, ctx->file, s_hdr);
			ELF_NAMED(parse_sym_hdr)(ctx, ptr_add(ctx->file->buffer, s_hdr->sh_offset), s_hdr);
		}
		s_hdr = next_hdr(s_hdr, ELF_INFO->e_shentsize, ctx->file);
		if (!s_hdr)
			out_of_range(ctx);
	}
}

#endif

#undef ELF_GLUE_I
#undef ELF_GLUE
#undef ELF_NAMED

#undef ELF_INFO

#undef ELF_POSTFIX
#undef ELF_SHDR
#undef ELF_SYM
#undef ELF_EHDR
