
static char
ELF_NAMED(get_symbol_type_from_s_hdr)(ELF_SHDR* symbol_relation_section, uint32_t s_hdr_type, bool is_maj) {
	uint64_t const s_hdr_flags = symbol_relation_section->sh_flags;

	switch (s_hdr_type) {
		case SHT_NOBITS:
			return (convert_gloal_symbol('b', is_maj));
		case SHT_PROGBITS: case SHT_NOTE: case SHT_DYNAMIC: case SHT_INIT_ARRAY: case SHT_FINI_ARRAY:
			return (convert_gloal_symbol(get_type_from_progbits(s_hdr_flags), is_maj));
	}
	return (convert_gloal_symbol('u', is_maj));
}

char
ELF_NAMED(get_symbos_type)(t_ctx* ctx, ELF_SYM* sym_hdr) {
	ELF_SHDR*	symbol_relation_section;
	bool		is_maj;
	char		result;

	if (ELF32_ST_BIND(sym_hdr->st_info) == STB_GNU_UNIQUE)
		return ('u');
	is_maj = (ELF32_ST_BIND(sym_hdr->st_info) == STB_GLOBAL);
	result = get_specifique_symbol_type(ctx, ELF_INFO->e_shnum, sym_hdr->st_shndx, sym_hdr->st_info);
	if (result)
		return (result);
	symbol_relation_section = (ELF_SHDR*)ptr_add(ctx->file->buffer, ELF_INFO->e_shoff) + sym_hdr->st_shndx;
	return (ELF_NAMED(get_symbol_type_from_s_hdr)(symbol_relation_section, symbol_relation_section->sh_type, is_maj));
}
