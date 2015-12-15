#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

struct ENTRY_LIST {
	struct MATRIX_ENTRY *entry;
	struct ENTRY_LIST *next;
};

uint32_t size_x, size_y;

struct MATRIX_ENTRY** row_start;
struct MATRIX_ENTRY** col_start;

struct ENTRY_LIST *entry_list = NULL;

void loop_col(int col){
	struct MATRIX_ENTRY* c = col_start[col];
	if(c){
		struct MATRIX_ENTRY* n = c;
		while(n->down){
			n = n->down;
		}
		n->down = c;
		c->up = n;
	}
}

void loop_row(int row){
	struct MATRIX_ENTRY* r = row_start[row];
	if(r){
		struct MATRIX_ENTRY* n = r;
		while(n->right){
			n = n->right;
		}
		n->right = r;
		r->left = n;
	}
}

void define_matrix(uint32_t x, uint32_t y){
	size_x = x;
	size_y = y;
	row_start = calloc(y, sizeof(struct MATRIX_ENTRY*));
	col_start = calloc(x, sizeof(struct MATRIX_ENTRY*));
}

void build_starts() {
	struct ENTRY_LIST *node = entry_list;
	while(node){
		struct MATRIX_ENTRY* e = node->entry;
		struct MATRIX_ENTRY* r = row_start[e->y];
		if(r){
			if(r->x > e->x){
				row_start[e->y] = e;
			}
		} else {
			row_start[e->y] = e;
		}
		struct MATRIX_ENTRY* c = col_start[e->x];
		if(c){
			if(c->y > e->y){
				col_start[e->x] = e;
			}
		} else {
			col_start[e->x] = e;
		}
		node = node->next;
	}
}

void link_matrix(){
	struct ENTRY_LIST *node = entry_list;
	while(node){
		struct MATRIX_ENTRY* e = node->entry;
		struct MATRIX_ENTRY* r = row_start[e->y];
		if(r!=e){
			while(r->right&&r->right->x < e->x){
				r = r->right;
			}
			if(r->right){
				e->right = r->right;
				e->right->left = e;
			}
			e->left = r;
			r->right = e;
		}
		struct MATRIX_ENTRY* c = col_start[e->x];
		if(c!=e){
			while(c->down&&c->down->y < e->y){
				c = c->down;
			}
			if (c->down) {
				e->down = c->down;
				e->down->up = e;
			}
			e->up = c;
			c->down = e;
		}
		node = node->next;
	}
	int i;
	for(i=0; i<size_x; i++){
		loop_col(i);
	}
	for(i=0; i < size_y; i++){
		loop_row(i);
	}
}

void print_matrix() {
	int x, y;
	for(y = 0; y < size_y; y++ ){
		struct MATRIX_ENTRY* r = row_start[y];
		struct MATRIX_ENTRY* e = r;
		for(x = 0; x < size_x; x++ ){
			if (e){
				if (e->x==x) {
					printf("1\t");
					e = e->right;
					if(e==r){
						e=NULL;
					}
				} else {
					printf("0\t");
				}
			} else { // Reached end of entries for row.
				while(x < size_x){
					printf("0\t");
					x++;
				}
			}
		}
		printf("\n");
	}
}

void add_entry(uint32_t x, uint32_t y){
	struct MATRIX_ENTRY *entry = calloc(1, sizeof(struct MATRIX_ENTRY));
	entry->x = x;
	entry->y = y;
	if (x < 0 || y < 0 || x >= size_x || y >= size_y){
		fprintf(stderr, "Error: (%d, %d) is Out of bounds\n", x,y);
		exit(EXIT_FAILURE);
	}
	struct ENTRY_LIST *list_object = malloc(sizeof(struct MATRIX_ENTRY));
	list_object->entry = entry;
	list_object->next = NULL;
	if(entry_list){
		struct ENTRY_LIST *iterator = entry_list;
		while(iterator->next){
			iterator = iterator->next;
		}
		iterator->next = list_object;
	} else {
		entry_list = list_object;
	}
}

void free_matrix(){
	while(entry_list){
		free(entry_list->entry);
		struct ENTRY_LIST *last_node = entry_list;
		entry_list = entry_list->next;
		free(last_node);
	}
}

void clean_matrix(void){
	if(row_start){
		free(row_start);
		row_start=NULL;
	}
	if(col_start){
		free(col_start);
		col_start=NULL;
	}
	free_matrix();
}

void column_append(struct MATRIX_COL* head, struct MATRIX_COL* new){
	while(head->right){
		head = head->right;
	}
	head->right = new;
	new->left = head;
	new->right = NULL;
}

void row_append(struct MATRIX_ROW* head, struct MATRIX_ROW* new){
	while (head->down) {
		head = head->down;
	}
	head->down = new;
	new->up = head;
	new->down = NULL;
}

int column_cardinality(struct MATRIX_COL* col){
	int c = 0;
	struct MATRIX_ENTRY *e = col->top;
	int s;
	if(e) {
		s = e->y;
		c = 1;
		e = e->down;
	}
	while(e && (e->y != s)){
		c++;
		e = e->down;
	}
	return c;
}

struct MATRIX* get_matrix(void){
	struct MATRIX* m = malloc(sizeof(struct MATRIX));
	m->xLen = size_x;
	m->yLen = size_y;
	m->entries = entry_list;
	struct MATRIX_COL* chead = malloc(sizeof(struct MATRIX_COL));
	chead->index = 0;
	chead->top = col_start[0];
	chead->cardinality = column_cardinality(chead);
	int min_cardinality = chead->cardinality;
	chead->right = NULL;
	int i;
	for (i=1; i < size_x; i++){
		struct MATRIX_COL* col = malloc(sizeof(struct MATRIX_COL));
		col->index = i;
		col->top = col_start[i];
		col->cardinality = column_cardinality(col);
		if(col->cardinality < min_cardinality){
			min_cardinality = col->cardinality;
		}
		column_append(chead, col);
	}
	struct MATRIX_COL* clast = chead;
	while(clast->right){
		clast = clast->right;
	}
	clast->right = chead;
	chead->left = clast;
	m->columns = chead;
	m->min_cardinality = min_cardinality;
	struct MATRIX_ROW* rhead = malloc(sizeof(struct MATRIX_ROW));
	rhead->index = 0;
	rhead->start = row_start[0];
	rhead->down = NULL;
	for (i=1; i < size_y; i++){
		struct MATRIX_ROW* row = malloc(sizeof(struct MATRIX_ROW));
		row->index = i;
		row->start = row_start[i];
		row_append(rhead, row);
	}
	struct MATRIX_ROW* rlast = rhead;
	while(rlast->down){
		rlast = rlast->down;
	}
	rlast->down = rhead;
	rhead->up = rlast;
	m->rows = rhead;
	return m;
}

void recalculate_cardinality(struct MATRIX* matrix) {
	struct MATRIX_COL* col = matrix->columns;
	if(!col){
		return;
	}
	int min_cardinality = INT_MAX;
	do{
		col->cardinality = column_cardinality(col);
		if(min_cardinality > col->cardinality){
			min_cardinality = col->cardinality;
		}
	} while (col != matrix->columns);
	matrix->min_cardinality = min_cardinality;
}

struct MATRIX_ENTRY* copy_matrix_entries(size_t *size){
	struct ENTRY_LIST* list = entry_list;
	*size = 0;
	while(list){
		(*size)++;
		list = list->next;
	}
	struct MATRIX_ENTRY* entries = calloc(*size, sizeof(struct MATRIX_ENTRY));
	list = entry_list;
	int i = 0;
	while(list){
		entries[i++] = *(list->entry);
		list = list->next;
	}
	return entries;
}

uint32_t get_matrix_height(void){
	return size_y;
}
uint32_t get_matrix_width(void){
	return size_x;
}
