#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

struct MATRIX_ENTRY {
	int x, y;
	struct MATRIX_ENTRY *up, *down, *left, *right;
};

void define_matrix(uint32_t x, uint32_t y);
void add_entry(uint32_t x, uint32_t y);
void build_starts(void);
void print_matrix(void);
void link_matrix();
void clean_matrix(void);

#endif /* MATRIX_H */
