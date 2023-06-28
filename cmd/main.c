//
// Created by millefeuille on 5/31/23.
//

#include "ft_nm.h"

// NOTE Test better for symbols types ?
// TODO flags g u r p

elf32_h *get_32_header(int fd) {
	elf32_h *header;
	header = mmap(NULL, sizeof(elf_h), PROT_READ, MAP_PRIVATE, fd, 0);
	return header;
}

# if defined(FT_NM_DEV)
int AArchF(main)(int fd, struct stat* file_stat, char *filename, char flags) {
	ArchF(main)(fd, file_stat, filename, flags);
	return 0;
}
#endif

int get_file_info(char *file, int *fd, struct stat *file_stat) {
	*fd = open(file, O_RDONLY);
	if (errno)
		return errno;

	if (fstat(*fd, file_stat) < 0) {
		close(*fd);
		return errno;
	}

	if (!S_ISREG(file_stat->st_mode)) {
		ft_fputstr("ft_nm", 2);
		ft_fputstr(": Warning: '", 2);
		ft_fputstr(file, 2);
		ft_fputstr("' is not an ordinary file\n", 2);
		close(*fd);
		return 1;
	}

	return 0;
}

int parse_file(char *file, char flags) {
	(void) flags;
	int fd = 0;
	struct stat file_stat;

	int err = get_file_info(file, &fd, &file_stat);
	if (err)
		return err;

	elf32_h *header = get_32_header(fd);
	if (header->e_ident[EI_CLASS] == ELFCLASS32) {
		err = f32_main(fd, &file_stat, file, flags);
	} else {
		err = f64_main(fd, &file_stat, file, flags);
	}

	if (err < 0) {
		ft_fputstr("ft_nm", 2);
		ft_fputstr(": ", 2);
		ft_fputstr(file, 2);
		ft_fputstr(": no symbols\n", 2);
	}

	close(fd);
	return 0;
}

int main(int argc, char **argv) {
	nm_args args = parse_args(argc, argv);
	if (args.err) {
		if (args.err == -2)
			return 1;
		panic("allocation");
	}

	if (!args.files) {
		char *file = ft_string("a.out");
		if (!file)
			panic("allocation");
		args.files = new_list_element(file);
		if (!args.files) {
			safe_free((void **) &file);
			panic("allocation");
		}
	}

	ft_list *current = args.files;
	for (; current; current = current->next) {
		parse_file(current->data, args.flags);
		if (errno) {
			log_error(current->data);
		}
	}

	return errno;
}
