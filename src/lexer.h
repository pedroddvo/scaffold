#ifndef LEXER_H
#define LEXER_H

#include "error.h"

struct token {
  enum {
    TOKEN_ERROR,
    TOKEN_EOF,
    TOKEN_SOF,

    TOKEN_NUMBER,
    TOKEN_SYMBOL,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_SLASH,
    TOKEN_STAR
  } id;

  int start;
  int offset;
};

struct lexer {
  char* src;
  int   srcLen;

  struct stacktrace trace;

  int srcOffset;
};

void lexer_init(struct lexer* l, char* src, int srcLen);
void lexer_token(struct lexer* l, struct token* t);
void lexer_free(struct lexer* l);


#endif
