#include "dlx.h"
#include <stdlib.h>

void dlx_delink_vertical(struct MATRIX_ENTRY* node);
void dlx_delink_horizontal(struct MATRIX_ENTRY* node);
void dlx_relink_vertical(struct MATRIX_ENTRY* node);
void dlx_relink_horizontal(struct MATRIX_ENTRY* node);

void dlx_remove_row(struct MATRIX_ROW** index, struct MATRIX_COL** cols, int id, int col_id);
void dlx_remove_column(struct DLX_STACK ** row_stack, struct MATRIX* matrix, struct MATRIX_COL** index, struct MATRIX_ROW** rows, int id);

void dlx_add_row(struct MATRIX_ROW** index, int id);
void dlx_add_column(struct MATRIX* matrix, struct MATRIX_COL** index, int id);

void dlx_stack_push(struct DLX_STACK **stack, int value){
  struct DLX_STACK *entry = malloc(sizeof(struct DLX_STACK));
  entry->id = value;
  entry->next = *stack;
  *stack = entry;
}

int dlx_stack_pop(struct DLX_STACK **stack){
  struct DLX_STACK *entry = *stack;
  int value = entry->id;
  *stack = entry->next;
  free(entry);
  return value;
}

struct DLX_STACK* dlx(struct MATRIX* matrix){
  struct MATRIX_COL** column_index = calloc(matrix->xLen, sizeof(struct MATRIX_COL*));
  struct MATRIX_ROW** row_index = calloc(matrix->yLen, sizeof(struct MATRIX_ROW*));
  struct MATRIX_COL* col = matrix->columns;
  do {
    column_index[col->index] = col;
    col = col->right;
  } while (col->index!=0);
  struct MATRIX_ROW* row = matrix->rows;
  do {
    row_index[row->index] = row;
    row = row->down;
  } while (row->index!=0);
  struct DLX_STACK *row_check_stack = NULL;
  struct DLX_STACK *row_solution_stack = NULL;
  struct DLX_STACK *column_stack = NULL;
  struct DLX_STACK *row_remove_stack = NULL;
  do {
    if(!matrix->columns){
      break;
    }
    col = matrix->columns;
    while(col->cardinality != matrix->min_cardinality){
      col = col->right;
    }
    struct MATRIX_ENTRY* entry = col->top;
    // Minus 1 means reached end of rows available. Should backtrack.
    // As this is a stack, this gets entered before the rows available.
    dlx_stack_push(&row_check_stack, -1);
    do {
      dlx_stack_push(&row_check_stack, entry->y);
      entry = entry->down;
    } while (entry!=col->top);
    if(!row_check_stack){
      break;
    }
    int row_id = dlx_stack_pop(&row_check_stack);
    while(row_id==-1){
      dlx_stack_pop(&row_solution_stack);
      int replace_id = dlx_stack_pop(&column_stack);
      while(replace_id!=-1){
        dlx_add_column(matrix, column_index, replace_id);
        replace_id = dlx_stack_pop(&column_stack);
      }
      replace_id = dlx_stack_pop(&row_remove_stack);
      while(replace_id!=-1){
        dlx_add_row(row_index, replace_id);
        replace_id = dlx_stack_pop(&row_remove_stack);
      }
      row_id = dlx_stack_pop(&row_check_stack);
    }
    dlx_stack_push(&row_solution_stack, row_id);
    dlx_remove_row(row_index, column_index, row_id, -1);
    row = row_index[row_id];
    // Again, Minus 1 is used to mark the end of a list.
    dlx_stack_push(&column_stack, -1);
    dlx_stack_push(&row_remove_stack, -1);
    struct MATRIX_ENTRY* node = row->start;
    do {
      dlx_remove_column(&row_remove_stack, matrix, column_index, row_index, node->x);
      dlx_stack_push(&column_stack, node->x);
      node = node->right;
    } while (node != row->start);
    if(matrix->columns){
      recalculate_cardinality(matrix);
    }
  } while(matrix->columns);
  free(column_index);
  free(row_index);
  return row_solution_stack;
}

void dlx_remove_row(struct MATRIX_ROW** index, struct MATRIX_COL** cols, int id, int col_id){
  struct MATRIX_ROW* row = index[id];
  row->down->up = row->up;
  row->up->down = row->down;
  struct MATRIX_ENTRY* node = row->start;
  do{
    dlx_delink_vertical(node);
    if(node->x != col_id && cols[node->x]->top == node){
      cols[node->x]->top = node->down;
    }
    node = node->right;
  } while (node != row->start);
}

void dlx_remove_column(struct DLX_STACK ** row_stack, struct MATRIX* matrix, struct MATRIX_COL** index, struct MATRIX_ROW** rows, int id){
  struct MATRIX_COL* col = index[id];
  col->left->right = col->right;
  col->right->left = col->left;
  if(col == matrix->columns){
    if(col->right != col){
      matrix->columns = col->right;
    } else {
      matrix->columns = NULL;
    }
  }
  struct DLX_STACK* rows_to_remove = NULL;
  struct MATRIX_ENTRY* node = col->top;
  do{
    dlx_stack_push(&rows_to_remove, node->y);
    dlx_stack_push(row_stack, node->y);
    node = node->down;
  } while (node != col->top);
  while(rows_to_remove){
    int r_id = dlx_stack_pop(&rows_to_remove);
    dlx_remove_row(rows, index, r_id, id);
  }
}

void dlx_delink_vertical(struct MATRIX_ENTRY* node){
  node->up->down = node->down;
  node->down->up = node->up;
}

void dlx_delink_horizontal(struct MATRIX_ENTRY* node){
  node->right->left = node->left;
  node->left->right = node->right;
}

void dlx_relink_vertical(struct MATRIX_ENTRY* node){
  node->up->down = node;
  node->down->up = node;
}

void dlx_relink_horizontal(struct MATRIX_ENTRY* node){
  node->right->left = node;
  node->left->right = node;
}

void dlx_add_row(struct MATRIX_ROW** index, int id){
  struct MATRIX_ROW* row = index[id];
  row->down->up = row;
  row->up->down = row;
  struct MATRIX_ENTRY* node = row->start;
  do{
    dlx_relink_vertical(node);
    node = node->right;
  } while (node != row->start);
}

void dlx_add_column(struct MATRIX* matrix, struct MATRIX_COL** index, int id){
  struct MATRIX_COL* col = index[id];
  col->left->right = col;
  col->right->left = col;
  if(!matrix->columns || matrix->columns->index > id){
    matrix->columns = col;
  }
}
