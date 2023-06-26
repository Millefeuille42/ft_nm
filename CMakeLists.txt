cmake_minimum_required(VERSION 3.10)
project(ft_nm C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -Werror -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -Wextra -pedantic")

add_definitions(-DFT_NM_DEV=1 -DFT_NM_BUILD_64=1)

set(SOURCE_FT_NM
		cmd/main.c
		cmd/ft_nm.h

		pkg/ft_memory/del_array.c
		pkg/ft_memory/ft_bzero.c
		pkg/ft_memory/ft_memory.h
		pkg/ft_memory/safe_free.c
		pkg/ft_memory/zeroed_malloc.c

		pkg/ft_string/ft_string_copy.c
		pkg/ft_string/ft_string_concat.c
		pkg/ft_string/get_after_n_sep.c
		pkg/ft_string/ft_strcmp.c
		pkg/ft_string/ft_to_lower.c
		pkg/ft_string/ft_strlen.c
		pkg/ft_string/ft_is_alnum.c
		pkg/ft_string/ft_string.h
		pkg/ft_string/ft_split.c
		pkg/ft_string/ft_is_alpha.c
		pkg/ft_string/string.c

		pkg/ft_print/ft_putchar.c
		pkg/ft_print/ft_putstr.c
		pkg/ft_print/ft_putnbr_base.c
		pkg/ft_print/ft_putnbr.c
		pkg/ft_print/ft_print.h
		pkg/ft_print/ft_putnbr_base_padded.c
		cmd/var_arch.c cmd/utils.c)

add_executable(ft_nm ${SOURCE_FT_NM})
target_link_libraries(ft_nm LINK_PUBLIC)