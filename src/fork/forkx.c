#include "forkx.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clean_row(struct MATRIX* matrix, int row_id);
void remove_column(struct MATRIX* matrix, int col_id);

#define INT_DEC_LEN 10

pid_t pid;

struct STACK {
  int value;
  struct STACK* next;
};

void push_stack(struct STACK** stack, int value){
  struct STACK* new = malloc(sizeof(struct STACK));
  new->value = value;
  new->next = *stack;
  *stack = new;
}

int pop_stack(struct STACK** stack){
  struct STACK* old = *stack;
  int value = old->value;
  *stack = old->next;
  free(old);
  return value;
}

int forkx(struct MATRIX* matrix) {
  int* results = calloc(matrix->yLen, sizeof(int));
  int result_count = 0;
  while(matrix->columns){
    struct MATRIX_COL* col = matrix->columns;
    if(matrix->min_cardinality==0){
      if(col){
        do{
          col = col->right;
          free(col->left);
        } while (col != matrix->columns);
      }
      struct MATRIX_ROW* row = matrix->rows;
      if(row){
        do{
          row = row->down;
          free(row->up);
        } while (row != matrix->rows);
      }
      free(results);
      return 0;
    }
    while(col->cardinality != matrix->min_cardinality){
      col = col->right;
    }
    struct MATRIX_ENTRY* entry = col->top;
    pid = 0;
    while(entry->down != col->top && pid == 0){
      pid = fork();
      if(pid==0){
        entry = entry->down;
      }
    }
    int row_id = entry->y;
    results[result_count] = row_id;
    result_count++;
    clean_row(matrix, row_id);
    recalculate_cardinality(matrix);
  }
  char* results_str = calloc(INT_DEC_LEN*result_count+1, sizeof(char));
  int i;
  for (i=0; i < result_count; i++){
    char tmp_str[INT_DEC_LEN];
    sprintf(tmp_str, "%d ", results[i]);
    strcat(results_str, tmp_str);
  }
  free(results);
  printf("%s\n", results_str);
  free(results_str);
  return 1;
}

void clean_row(struct MATRIX* matrix, int row_id){
  struct MATRIX_ROW* row = matrix->rows;
  while(row->index != row_id){
    row = row->down;
  }
  struct MATRIX_ENTRY* row_entry = row->start;
  do{
    struct MATRIX_ENTRY* col_entry = row_entry->down;
    struct STACK* to_remove = NULL;
    while(col_entry != row_entry){
      push_stack(&to_remove, col_entry->y);
      col_entry = col_entry->down;
    }
    while(to_remove){
      int rid = pop_stack(&to_remove);
      struct MATRIX_ROW* rem_row = matrix->rows;
      while(rem_row->index != rid){
        rem_row = rem_row->down;
      }
      struct MATRIX_ENTRY* rem_entry = rem_row->start;
      do {
        rem_entry->up->down = rem_entry->down;
        rem_entry->down->up = rem_entry->up;
        if(rem_entry->up->y >= rem_entry->y) {
          struct MATRIX_COL* col_to_fix = matrix->columns;
          while(col_to_fix->index != rem_entry->x){
            col_to_fix = col_to_fix->right;
          }
          if(rem_entry->down==rem_entry){
            col_to_fix->top = NULL;
          } else {
            col_to_fix->top = rem_entry->down;
          }
        }
        rem_entry = rem_entry->right;
      } while(rem_entry != rem_row->start);
      rem_row->up->down = rem_row->down;
      rem_row->down->up = rem_row->up;
      if(matrix->rows == rem_row){
        if(rem_row->down == rem_row){
          matrix->rows == NULL;
        } else {
          matrix->rows = rem_row->down;
        }
      }
      free(rem_row);
    }
    remove_column(matrix, row_entry->x);
    row_entry = row_entry->right;
  } while(row_entry != row->start);
  row->up->down = row->down;
  row->down->up = row->up;
  if(row == matrix->rows){
    if(row->down == row){
      matrix->rows == NULL;
    } else {
      matrix->rows = row->down;
    }
  }
  free(row);
}

void remove_column(struct MATRIX* matrix, int col_id){
  struct MATRIX_COL* col = matrix->columns;
  while(col->index != col_id){
    col = col->right;
  }
  col->right->left = col->left;
  col->left->right = col->right;
  if(matrix->columns == col){
    if(col->right == col){
      matrix->columns = NULL;
    } else {
      matrix->columns = col->right;
    }
  }
  free(col);
}
