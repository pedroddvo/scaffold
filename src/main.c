#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("scaffold.exe expects an argument\n");
    return 1;
  }
  char* src = argv[1];
  

  struct chunk chunk;
  parse(src, strlen(src), &chunk);
  chunk_debug(&chunk);

  struct vm vm;
  vm_execute(&vm, &chunk);
  printf("%f\n", vm_top(&vm));

  vm_free(&vm);
  chunk_free(&chunk);
  return 0;
}
