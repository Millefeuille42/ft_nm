//
// Created by millefeuille on 5/31/23.
//

#include "ft_nm.h"

// TODO Test better for symbols types
// TODO Add multi file support
// TODO Add flag support

elf32_h *get_32_header(int fd) {
	elf32_h *header;
	header = mmap(NULL, sizeof(elf_h), PROT_READ, MAP_PRIVATE, fd, 0);
	return header;
}

# if defined(FT_NM_DEV)
int AArchF(main)(int fd, struct stat* file_stat) {
	(void)fd;
	(void)file_stat;
	return 0;
}
#endif

int main(int argc, char **argv) {
	if (argc <= 1) {
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (errno) {
		ft_fputstr(argv[0], 2);
		ft_fputstr(": ", 2);
		ft_fputstr(argv[1], 2);
		ft_fputstr(": could not access file\n", 2);
		close(fd);
		return errno;
	}

	struct stat file_stat;
	if (fstat(fd, &file_stat) < 0) {
		close(fd);
		return errno;
	}

	if (!S_ISREG(file_stat.st_mode)) {
		ft_fputstr(argv[0], 2);
		ft_fputstr(": Warning: '", 2);
		ft_fputstr(argv[1], 2);
		ft_fputstr("' is not an ordinary file\n", 2);
		return 1;
	}

	elf32_h *header = get_32_header(fd);
	int err;
	if (header->e_ident[EI_CLASS] == ELFCLASS32) {
		err = f32_main(fd, &file_stat);
	}
	else {
		err = f64_main(fd, &file_stat);
	}

	if (err < 0) {
		ft_fputstr(argv[0], 2);
		ft_fputstr(": ", 2);
		ft_fputstr(argv[1], 2);
		ft_fputstr(": no symbols\n", 2);
	}

	close(fd);
	return 0;
}
