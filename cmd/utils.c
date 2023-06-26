//
// Created by millefeuille on 6/26/23.
//

#include "ft_nm.h"

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
