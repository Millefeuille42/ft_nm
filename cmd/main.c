#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

#if defined(__LP64__)
# define ElfW(type) Elf64_ ## type
#else
# define ElfW(type) Elf32_ ## type
#endif


typedef ElfW(Ehdr) elf_h;
typedef ElfW(Shdr) elf_sh;

elf_h get_header(const int fd) {
	elf_h header;
	ssize_t ret = read(fd, &header, sizeof(header));
	if (ret < (ssize_t) sizeof(header)) errno = EINVAL;
	return header;
}

elf_sh get_section_header(const int fd) {
	elf_sh s_header;
	ssize_t ret = read(fd, &s_header, sizeof(s_header));
	if (ret < (ssize_t) sizeof(s_header)) errno = EINVAL;
	return s_header;
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

int main(void) {
	int fd = open("ft_nm", O_RDONLY);
	if (errno) return errno;

	elf_h header = get_header(fd);
	if (errno) return errno;

	if (!check_signature(&header)) return errno;

	close(fd);
	return 0;
}
