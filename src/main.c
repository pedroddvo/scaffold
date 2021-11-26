#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code.h"
#include "lexer.h"
#include "parser.h"

int main() {
  char src[] = "1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12 * 13 + 14";
  

  struct chunk chunk;
  parse(src, strlen(src), &chunk);

  chunk_debug(&chunk);

  chunk_free(&chunk);
  return 0;
}
