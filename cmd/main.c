#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <string.h>

#include "../pkg/ft_memory/ft_memory.h"
#include "../pkg/ft_string/ft_string.h"

#if defined(__LP64__)
# define ElfW(type) Elf64_ ## type
#else
# define ElfW(type) Elf32_ ## type
#endif

// TODO Add 32Bits support
// TODO Add invalid file support (segfault)
// TODO Fix sorting
// TODO Test better for symbols types

typedef ElfW(Ehdr) elf_h;
typedef ElfW(Shdr) elf_sh;
typedef ElfW(Sym) elf_sym;

elf_h *get_header(int fd) {
	elf_h *header;
	header = mmap(NULL, sizeof(elf_h), PROT_READ, MAP_PRIVATE, fd, 0);
	return header;
}

int check_signature(elf_h *header) {
	for (int i = 0; i < SELFMAG; i++) {
		if (ELFMAG[i] != header->e_ident[i]) {
			errno = EINVAL;
			return 0;
		}
	}
	return 1;
}

void ft_swap(void **x, void **y) {
	void *temp = *x;
	*x = *y;
	*y = temp;
}

int num_sort(void *a, void *b) {
	ssize_t *a_num = a;
	ssize_t *b_num = b;

	return *b_num > *a_num;
}

static int is_alpha_valid(char c) {
	return ft_is_alnum(c) || c == '$';
}

int string_alpha_sort(void *a, void *b) {
	char *a_string = a;
	char *b_string = b;

	size_t a_i = 0;
	size_t b_i = 0;

	for (; a_string[a_i] && b_string[b_i];) {
		for (; a_string[a_i] && !is_alpha_valid(a_string[a_i]); a_i++);
		for (; b_string[b_i] && !is_alpha_valid(b_string[b_i]); b_i++);
		char a_char = (char) ft_tolower(a_string[a_i]);
		char b_char = (char) ft_tolower(b_string[b_i]);
		if (a_char != b_char)
			return (char) (a_char < b_char);
		a_i++;
		b_i++;
	}
	return ft_strlen(a_string) < ft_strlen(b_string);
}

int dumb_alpha_sort(void *a, void *b) {
	char *a_string = a;
	char *b_string = b;

	int i = 0;
	for (; a_string[i] && b_string[i]; i++) {
		if (a_string[i] != b_string[i])
			return (char) (a_string[i] < b_string[i]);
	}

	if (!a_string[i])
		return 1;
	return 0;
}


void selection_sort(elf_sym **arr, size_t size, char *strtab) {
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
char symtab_to_letter(elf_sym *symtab, elf_sh *sections) {
	size_t bind = ELF64_ST_BIND(symtab->st_info);
	size_t type = ELF64_ST_TYPE(symtab->st_info);

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

void parse_symtab(elf_sh *sh_strtab, elf_sh *sh_symtab, char *buf, elf_sh *sections) {
	elf_sym *symtab = (elf_sym *) (buf + sh_symtab->sh_offset);
	char *strtab = buf + sh_strtab->sh_offset;
	size_t size = sh_symtab->sh_size / sh_symtab->sh_entsize;

	elf_sym **symtab_arr = malloc(size * sizeof(elf_sym *));
	if (!symtab_arr) return;

	for (size_t i = 0; i < size; i++) {
		symtab_arr[i] = symtab;
		symtab++;
	}

	selection_sort(symtab_arr, size, strtab);

	for (size_t i = 0; i < size; i++) {
		symtab = symtab_arr[i];
		size_t bind = ELF64_ST_BIND(symtab->st_info);
		size_t type = ELF64_ST_TYPE(symtab->st_info);

		if ((type == STT_FILE || (!type && !bind)) && symtab->st_value <= 0)
			continue;

		if (symtab->st_value > 0)
			printf("%.16lx", symtab->st_value);
		else
			printf("%16s", "");
		//printf(" %c %lu %lu %lu %s\n", symtab_to_letter(symtab, sections), type, bind, sh_symtab->sh_flags, strtab + symtab->st_name);
		printf(" %c %s\n", symtab_to_letter(symtab, sections), strtab + symtab->st_name);
	}
}

void parse_section_headers(elf_h *header, int fd, size_t size) {
	char *buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (errno) return;

	elf_sh *sections = (elf_sh *)(buf + header->e_shoff);
	elf_sh *sh_shstrtab = sections + header->e_shstrndx;
	char *shstrtab = buf + sh_shstrtab->sh_offset;

	elf_sh *sh_strtab;
	elf_sh *sh_symtab;

	for (size_t i = 0; i < header->e_shnum; i++) {
		if (errno) {
			munmap(buf, size);
			return;
		}
		if (!ft_strcmp(shstrtab + sections[i].sh_name, ".symtab"))
			sh_symtab = &sections[i];
		if (!ft_strcmp(shstrtab + sections[i].sh_name, ".strtab"))
			sh_strtab = &sections[i];
	}

	parse_symtab(sh_strtab, sh_symtab, buf, sections);
	munmap(buf, size);
}

int main(int argc, char **argv) {
	if (argc <= 1)
		return 1;

	int fd = open(argv[1], O_RDONLY);
	if (errno) {
		close(fd);
		return errno;
	}
	struct stat file_stat;
	if (fstat(fd, &file_stat) < 0) {
		close(fd);
		return errno;
	}

	sysconf(_SC_PAGE_SIZE);

	elf_h *header = get_header(fd);
	if (errno) {
		close(fd);
		return errno;
	}

	if (!check_signature(header)) {
		munmap(header, sizeof(elf_h));
		close(fd);
		return errno;
	}

	parse_section_headers(header, fd, file_stat.st_size);
	if (errno) {
		munmap(header, sizeof(elf_h));
		close(fd);
		return errno;
	}

	close(fd);
	return 0;
}
