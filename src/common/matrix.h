#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

struct MATRIX_ENTRY {
	int x, y;
	struct MATRIX_ENTRY *up, *down, *left, *right;
};

struct ENTRY_LIST;

struct MATRIX_COL {
	int index;
	int cardinality;
	struct MATRIX_ENTRY *top;
	struct MATRIX_COL *left, *right;
};

struct MATRIX_ROW {
	int index;
	struct MATRIX_ENTRY *start;
	struct MATRIX_ROW *up, *down;
};

struct MATRIX {
	uint32_t xLen, yLen;
	int min_cardinality;
	struct ENTRY_LIST *entries;
	struct MATRIX_COL *columns;
	struct MATRIX_ROW *rows;
};

void define_matrix(uint32_t x, uint32_t y);
void add_entry(uint32_t x, uint32_t y);
void build_starts(void);
void print_matrix(void);
void link_matrix(void);
void clean_matrix(void);
struct MATRIX* get_matrix(void);
struct MATRIX* clone_matrix(struct MATRIX* matrix);
void recalculate_cardinality(struct MATRIX*);
void free_matrix_clone(struct MATRIX* matrix);

#endif /* MATRIX_H */
