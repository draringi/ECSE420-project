#include <cuda.h>
extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "cudax.h"
}

__device__ void cardinalities_calculations (int* cardinalities, int* min_cardinality, struct MATRIX_ENTRY** cols, int col_count){
  int i;
  for (i=0; i < col_count; i++){
    if(cardinalities[i]==-1){
      continue;
    }
    cardinalities[i]=0;
    struct MATRIX_ENTRY* e = cols[i];
    if(e){
      do {
        cardinalities[i]++;
        e = e->down;
      } while(e!=cols[i]);
    }
    if(cardinalities[i]< (*min_cardinality)){
      *min_cardinality = cardinalities[i];
    }
  }
}

__global__ void solve_cover ( struct MATRIX_ENTRY* entries, int* results , size_t list_size, uint32_t width, uint32_t height) {
  size_t results_base = blockIdx.x*height;
  size_t results_end = results_base + height;
  size_t entry_base = blockIdx.x*list_size;
  size_t entry_end = entry_base + list_size;
  size_t i;
  struct MATRIX_ENTRY** cols = (struct MATRIX_ENTRY**) malloc(width * sizeof(struct MATRIX_ENTRY*));
  for(i = 0; i < width; i++){
    cols[i] = NULL;
  }
  struct MATRIX_ENTRY** rows = (struct MATRIX_ENTRY**) malloc(height * sizeof(struct MATRIX_ENTRY*));
  for(i=0; i < height; i++){
    rows[i] = NULL;
  }
  // Find start of each row and column.
  for(i=entry_base; i<entry_end; i++){
    int col_id = entries[i].x;
    int row_id = entries[i].y;
    if(cols[col_id]){
      if(cols[col_id]->y>row_id){
        cols[col_id] = &entries[i];
      }
    } else {
      cols[col_id] = &entries[i];
    }
    if(rows[row_id]){
      if(rows[row_id]->x>col_id){
        rows[row_id] = &entries[i];
      }
    } else {
      rows[row_id] = &entries[i];
    }
  }
  //link
  for(i=entry_base; i<entry_end; i++){
    struct MATRIX_ENTRY* e = &entries[i];
		struct MATRIX_ENTRY* r = rows[e->y];
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
		struct MATRIX_ENTRY* c = cols[e->x];
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
  }
  for(i = 0; i < height; i ++){
    struct MATRIX_ENTRY* front = rows[i];
    struct MATRIX_ENTRY* end = front;
    while(end->right){
      end = end->right;
    }
    end->right = front;
    front->left = end;
  }
  for(i = 0; i < width; i++){
    struct MATRIX_ENTRY* top = cols[i];
    struct MATRIX_ENTRY* bottom = top;
    while(bottom->down){
      bottom = bottom->down;
    }
    bottom->down = top;
    top->up = bottom;
  }
  for(i = results_base; i < results_end; i++){
    results[i] = -1;
  }
  size_t result_count = 0;
  size_t columns_left = width;
  size_t rows_left = height;
  int* cardinalities = (int*) malloc(width*sizeof(int));
  int min_cardinality = INT_MAX;
  cardinalities_calculations(cardinalities, &min_cardinality, cols, width);
  //Solve;
  while(columns_left){
    if(!rows_left||min_cardinality==0){
      results[0] = -1;
      break;
    }
    i = 0;
    while (cardinalities[i] != min_cardinality){
      i++;
    }
    size_t to_choose = blockIdx.x % (min_cardinality+1);
    size_t row_count;
    struct MATRIX_ENTRY* e = cols[i];
    for (row_count = 0; row_count < to_choose; row_count++){
      e = e->down;
    }
    struct MATRIX_ENTRY* r = rows[e->y];
    do{
      struct MATRIX_ENTRY* c = r->down;
      while(r != c){
        struct MATRIX_ENTRY* rem = c->right;
        while (rem != c){
          rem->up->down = rem->down;
          rem->down->up = rem->up;
          if(rem == cols[rem->x]){
            if(rem->down == rem){
              cols[rem->x] = NULL;
            } else {
              cols[rem->x] = rem->down;
            }
          }
          rem = rem->right;
        }
        rows[c->y] = NULL;
        c = c->down;
        --rows_left;
      }
      cols[r->x] = NULL;
      cardinalities[r->x] = -1;
      r = r->right;
      --columns_left;
    } while (r != rows[e->y]);
    --rows_left;
    rows[e->y] = NULL;
    cardinalities_calculations(cardinalities, &min_cardinality, cols, width);
    results[results_base+result_count] = e->y;
    ++result_count;
  }
  free(cols);
  free(cardinalities);
  free(rows);
}

uint64_t factorial(uint32_t n){
  uint64_t value = 1;
  while (n > 1){
    value *= n;
    --n;
  }
  return n;
}

void find_cover(struct MATRIX_ENTRY* entries_list, size_t list_size, uint32_t width, uint32_t height){
  uint64_t* cardinalities = (uint64_t*)calloc(width, sizeof(uint64_t));
  size_t i;
  for(i=0; i<list_size; i++){
    int col = entries_list[i].x;
    cardinalities[col]++;
  }
  uint64_t cardinality_product = 1;
  for(i=0; i<list_size; i++){
    cardinality_product *= cardinalities[i];
  }
  uint64_t row_factorial = factorial(height);
  uint64_t cores = (cardinality_product > row_factorial) ? row_factorial : cardinality_product;
  void* data_array;
  size_t len = list_size*sizeof(struct MATRIX_ENTRY);
  size_t array_size = cores*len;
  cudaError_t err = cudaMalloc(&data_array, array_size);
  if(err != cudaSuccess){
    abort();
  }
  for(i = 0; i < cores; i++){
    void* ptr = (void*)(data_array + (i*len));
    err = cudaMemcpy(ptr, entries_list, len, cudaMemcpyHostToDevice);
    if(err != cudaSuccess){
      abort();
    }
  }
  size_t results_size = height*cores*sizeof(int);
  void* results;
  err = cudaMalloc(&results, results_size);
  if(err != cudaSuccess){
    abort();
  }
  solve_cover<<<cores, 1>>>((struct MATRIX_ENTRY*)data_array, (int*)results, list_size, width, height);
  int* local_results = (int*) malloc(results_size);
  err = cudaMemcpy(results, local_results, results_size, cudaMemcpyDeviceToHost);
  if(err != cudaSuccess){
    abort();
  }
  size_t success_count = 0;
  for (i=0; i<cores; i++){
    size_t res_length = 0;
    size_t j;
    size_t base = i*height;
    for(j = base; j < base + height; j++){
      int val = local_results[j];
      if(val == -1) {
        break;
      }
      printf("%d\t", val);
      res_length++;
    }
    if(res_length>0){
      printf("\n");
      success_count++;
    }
  }
  if(success_count == 0){
    printf("No Solutions Found\n");
  }
}
