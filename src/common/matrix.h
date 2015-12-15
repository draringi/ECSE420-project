#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>
#include <stdlib.h>
int yyparse(void);

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
void recalculate_cardinality(struct MATRIX*);
void free_matrix_object(struct MATRIX* matrix);
struct MATRIX_ENTRY* copy_matrix_entries(size_t *size);
uint32_t get_matrix_height(void);
uint32_t get_matrix_width(void);
#endif /* MATRIX_H */
