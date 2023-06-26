//
// Created by millefeuille on 6/26/23.
//

#include "ft_nm.h"

elf_h *ArchF(get_header)(int fd) {
	elf_h *header;
	header = mmap(NULL, sizeof(elf_h), PROT_READ, MAP_PRIVATE, fd, 0);
	return header;
}

int ArchF(check_signature)(elf_h *header) {
	for (int i = 0; i < SELFMAG; i++) {
		if (ELFMAG[i] != header->e_ident[i]) {
			errno = EINVAL;
			return 0;
		}
	}
	return 1;
}

void ArchF(selection_sort)(elf_sym **arr, size_t size, char *strtab) {
	for (size_t i = 0; i < size - 1; i++) {
		size_t min_i = i;

		for (size_t y = i + 1; y < size; y++) {
			int are_equal = !ft_strcmp(arr[y]->st_name + strtab, arr[min_i]->st_name + strtab);
			if (are_equal && num_sort(&arr[y]->st_value, &arr[min_i]->st_value))
				min_i = y;

			if (!are_equal && string_alpha_sort(arr[y]->st_name + strtab, arr[min_i]->st_name + strtab))
				min_i = y;
		}

		ft_swap((void **)&arr[min_i], (void **)&arr[i]);
	}
}

// DONE A b c d r t vV wW ?
// TODO g Ii n N p s
char ArchF(symtab_to_letter)(elf_sym *symtab, elf_sh *sections) {
	size_t bind = ELF_ST_BIND(symtab->st_info);
	size_t type = ELF_ST_TYPE(symtab->st_info);

	size_t section_index = symtab->st_shndx;
	elf_sh *symbol_section = &(sections[section_index]);
	int readonly = !(symbol_section->sh_flags & SHF_WRITE);

	if (symtab->st_shndx == SHN_ABS)
		return 'A';

	if (symbol_section->sh_type == SHT_NOBITS) {
		return bind == STB_LOCAL ? 'b' : 'B';
	}

	if (bind == STB_WEAK) {
		if (type == STT_OBJECT)
			return symtab->st_value <= 0 ? 'v' : 'V';
		return symtab->st_value <= 0 ? 'w' : 'W';
	}

	if (symtab->st_shndx == SHN_UNDEF)
		return 'U';

	if (type == STT_NOTYPE || type == STT_OBJECT) {
		if (readonly)
			return bind == STB_LOCAL ? 'r' : 'R';
		return bind == STB_LOCAL ? 'd' : 'D';
	}

	switch (type) {
		case STT_COMMON:
			return bind == STB_LOCAL ? 'c' : 'C';
		case STT_FUNC:
			return bind == STB_LOCAL ? 't' : 'T';
		default:
			break;
	}

	return '?';
}

void ArchF(parse_symtab)(elf_sh *sh_strtab, elf_sh *sh_symtab, char *buf, elf_sh *sections) {
	elf_sym *symtab = (elf_sym *) (buf + sh_symtab->sh_offset);
	char *strtab = buf + sh_strtab->sh_offset;
	size_t size = sh_symtab->sh_size / sh_symtab->sh_entsize;

	elf_sym **symtab_arr = malloc(size * sizeof(elf_sym *));
	if (!symtab_arr) return;

	for (size_t i = 0; i < size; i++) {
		symtab_arr[i] = symtab;
		symtab++;
	}

	ArchF(selection_sort)(symtab_arr, size, strtab);

	for (size_t i = 0; i < size; i++) {
		symtab = symtab_arr[i];
		size_t bind = ELF_ST_BIND(symtab->st_info);
		size_t type = ELF_ST_TYPE(symtab->st_info);

		if ((type == STT_FILE || (!type && !bind)) && symtab->st_value <= 0)
			continue;

		if (symtab->st_value > 0 || ArchF(symtab_to_letter)(symtab, sections) == 'T')
			//printf("%.16lx", symtab->st_value);
			ft_putnbr_base_padded(symtab->st_value, HEX_CHARSET, HEX_SIZE, '0', -ELF_ST_PAD_SIZE);
			//ft_putnbr_base(symtab->st_value, HEX_CHARSET, HEX_SIZE);
		else
			ft_putstr(ELF_ST_PAD_STR);
		//printf(" %c %lu %lu %lu %s\n", symtab_to_letter(symtab, sections), type, bind, sh_symtab->sh_flags, strtab + symtab->st_name);
		ft_putchar(' '),
		ft_putchar(ArchF(symtab_to_letter)(symtab, sections));
		ft_putchar(' '),
		ft_putstr(strtab + symtab->st_name);
		ft_putchar('\n');
	}
}

int ArchF(parse_section_headers)(elf_h *header, int fd, size_t size) {
	char *buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (errno) return 1;

	elf_sh *sections = (elf_sh *)(buf + header->e_shoff);
	elf_sh *sh_shstrtab = sections + header->e_shstrndx;
	char *shstrtab = buf + sh_shstrtab->sh_offset;

	elf_sh *sh_strtab = NULL;
	elf_sh *sh_symtab = NULL;

	for (size_t i = 0; i < header->e_shnum; i++) {
		if (errno) {
			munmap(buf, size);
			return 1;
		}
		if (!ft_strcmp(shstrtab + sections[i].sh_name, ".symtab"))
			sh_symtab = &sections[i];
		if (!ft_strcmp(shstrtab + sections[i].sh_name, ".strtab"))
			sh_strtab = &sections[i];
	}

	if (!sh_strtab || !sh_symtab) {
		munmap(buf, size);
		return -2;
	}

	ArchF(parse_symtab)(sh_strtab, sh_symtab, buf, sections);
	munmap(buf, size);
	return 0;
}

int ArchF(main)(int fd, struct stat* file_stat) {
	elf_h *header = ArchF(get_header)(fd);
	if (errno) {
		close(fd);
		return errno;
	}

	if (!ArchF(check_signature)(header)) {
		munmap(header, sizeof(elf_h));
		close(fd);
		return errno;
	}

	int err = ArchF(parse_section_headers)(header, fd, file_stat->st_size);
	if (errno) {
		munmap(header, sizeof(elf_h));
		close(fd);
		return errno;
	}
	if (err == -2) {
		return err;
	}

	return 0;
}
