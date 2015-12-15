#include "common/matrix.h"
#include "common/parser.yy.h"

#include <stdio.h>
#include <stdlib.h>
#include "cudax.h"


int main(int argc, char const *argv[]) {
  if(argc!=2){
    printf("USAGE: %s FILE\n", argv[0]);
    return EXIT_FAILURE;
  }
  FILE* f = fopen(argv[1], "r");
  if(!f){
    perror("Failed to open file: ");
    return EXIT_FAILURE;
  }
  atexit(clean_matrix);
  yyin = f;
  yyparse();
  fclose(f);
  yylex_destroy();
  size_t number_entries;
  struct MATRIX_ENTRY* entries = copy_matrix_entries(&number_entries);
  find_cover(entries, number_entries, get_matrix_width(), get_matrix_height());
  return EXIT_SUCCESS;
}
