#include "matrix.h"
#include "parser.yy.h"
#include <stdlib.h>

int main(int argc, char* argv[]){
  atexit(clean_matrix);
  FILE* f=NULL;
  if(argc>1){
    f = fopen(argv[1], "r");
    if(!f){
      perror("ERROR: Could not open file:");
      return EXIT_FAILURE;
    }
    yyin = f;
  }
  yyparse();
  if(f){
    fclose(f);
  }
  build_starts();
  link_matrix();
  print_matrix();
  return EXIT_SUCCESS;
}
