//
// Created by millefeuille on 5/31/23.
//

#ifndef FT_NM_FT_NM_H
# define FT_NM_FT_NM_H

# include <stdio.h>
# include <sys/mman.h>
# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <elf.h>
# include <sys/stat.h>
# include <string.h>

# include "../pkg/ft_memory/ft_memory.h"
# include "../pkg/ft_string/ft_string.h"
# include "../pkg/ft_print/ft_print.h"
# include "../pkg/ft_list/ft_list.h"
# include "../pkg/ft_error/ft_error.h"

# define NM_FLAG_a              1               // 00000001
# define NM_FLAG_g              2               // 00000010
# define NM_FLAG_u              4               // 00000100
# define NM_FLAG_r              8               // 00001000
# define NM_FLAG_p              16              // 00010000
# define NM_FLAG_dirs			32				// 00100000

# define NM_SET_FLAG(flags, flag) (flags |= flag)
# define NM_CLEAR_FLAG(flags, flag) (flags &= ~(flag))
# define NM_HAS_FLAG(flags, flag) (flags & flag)

# if defined(FT_NM_BUILD_64)
#  define ElfW(type) Elf64_ ## type
#  define ELF_ST_BIND ELF64_ST_BIND
#  define ELF_ST_TYPE ELF64_ST_TYPE
#  define ELF_ST_PAD_SIZE 16
#  define ELF_ST_PAD_STR "                "
#  define ArchF(func) f64_ ## func
#  define AArchF(func) f32_ ## func
# else
#  define ElfW(type) Elf32_ ## type
#  define ELF_ST_BIND ELF32_ST_BIND
#  define ELF_ST_TYPE ELF32_ST_TYPE
#  define ELF_ST_PAD_SIZE 8
#  define ELF_ST_PAD_STR "        "
#  define ArchF(func) f32_ ## func
#  define AArchF(func) f64_ ## func
# endif

# define HEX_CHARSET "0123456789abcdef"
# define HEX_SIZE 16

typedef ElfW(Ehdr) elf_h;
typedef ElfW(Shdr) elf_sh;
typedef ElfW(Sym) elf_sym;
typedef Elf32_Ehdr elf32_h;

typedef struct s_nm_args {
	ft_list *files;
	int err;
	char flags;
} nm_args;

void ft_swap(void **x, void **y);
int num_sort(void *a, void *b);
int string_alpha_sort(void *a, void *b);
nm_args parse_args(int argc, char **argv);

int ArchF(main)(int fd, struct stat *file_stat, char *filename, char flags);
int AArchF(main)(int fd, struct stat *file_stat, char *filename, char flags);

#endif //FT_NM_FT_NM_H
