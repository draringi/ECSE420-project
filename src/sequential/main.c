#include "common/matrix.h"
#include "common/parser.yy.h"
#include "dlx.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
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
  build_starts();
  link_matrix();
  struct MATRIX *matrix = get_matrix();
  struct DLX_STACK* solution = dlx(matrix);
  if(solution){
    while(solution){
      printf("%d\t", solution->id);
      struct DLX_STACK* tmp = solution;
      solution = solution->next;
      free(tmp);
    }
    printf("\n");
  } else {
    printf("No Solution Found\n");
  }

  return EXIT_SUCCESS;
}
