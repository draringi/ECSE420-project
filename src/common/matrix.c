#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>

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
