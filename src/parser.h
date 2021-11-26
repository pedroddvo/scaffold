#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "code.h"
#include "error.h"

struct tokenized {
  int len;
  int cap;
  struct token* tokens;
};

struct parser {
  struct tokenized  tokenized;
  struct stacktrace trace;
  struct chunk*     chunk;

  char* src;
  int tokOffset;
};

void parse(char* src, int srcLen, struct chunk* emitted);

#endif
