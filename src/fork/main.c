#include "forkx.h"
#include "common/matrix.h"
#include "common/parser.yy.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>


void wait_for_children(void);

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
  build_starts();
  link_matrix();
  struct MATRIX *matrix = get_matrix();
  forkx(matrix);
  wait_for_children();
  return EXIT_SUCCESS;
}

void wait_for_children(void){
  int status;
  do{
    wait(&status);
    if(status == -1 && errno != ECHILD){
      //Some other error occured;
      perror("Unknown Wait Error: ");
      return;
    }
  } while (status > 0);
}
