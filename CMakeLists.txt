cmake_minimum_required(VERSION 3.10)
project(ft_nm C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall -Werror -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -Wextra -pedantic")

set(SOURCE_FT_NM
		cmd/main.c
		cmd/ft_nm.h)

add_executable(ft_nm ${SOURCE_FT_NM})
target_link_libraries(ft_nm LINK_PUBLIC)