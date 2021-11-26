#include "lexer.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static bool eof(struct lexer* l) {
  return l->srcOffset >= l->srcLen;
}

static char peek(struct lexer* l) {
  if (eof(l)) return 0;
  return l->src[l->srcOffset];
}

static char next(struct lexer* l) {
  if (eof(l)) return 0;
  return l->src[l->srcOffset++];
}

static bool is_digit(char c) {
  return (c >= '0') && (c <= '9');
}

static bool is_alpha(char c) {
  return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

static bool is_alnum(char c) {
  return is_digit(c) || is_alpha(c);
}

static bool is_ws(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

void lexer_init(struct lexer* l, char* src, int srcLen) {
  l->src = src;
  l->srcLen = srcLen;

  l->srcOffset = 0;

  stacktrace_init(&l->trace);
}

void lexer_token(struct lexer* l, struct token* t) {
  if (eof(l)) { t->id = TOKEN_EOF; goto eof; }
  while (is_ws(peek(l)))
    next(l);
  if (eof(l)) { t->id = TOKEN_EOF; goto eof; }

  char c = peek(l);
  int start = l->srcOffset;

  if (is_digit(c)) {
    // read digits
    while (is_digit(peek(l))) {
      next(l);
    }

    if (peek(l) == '.') {
      next(l);
      while (is_digit(peek(l))) {
	next(l);
      }
    }
    t->id = TOKEN_NUMBER;
  } else if (is_alpha(c)) {
    // read alphanumeric symbols
    while (is_alnum(peek(l))) {
      next(l);
    }
    t->id = TOKEN_SYMBOL;
  }

  else {
    switch (c) {
    case '+': t->id = TOKEN_PLUS; break;
    case '-': t->id = TOKEN_MINUS; break;
    case '*': t->id = TOKEN_STAR; break;
    case '/': t->id = TOKEN_SLASH; break;
    case '(':
      t->id = TOKEN_LPAREN;
      l->parenCount++;
      l->parenLast = l->srcOffset;
      break;
    case ')':
      t->id = TOKEN_RPAREN;
      l->parenCount--;
      break;
    default:
      t->id = TOKEN_ERROR;
      stacktrace_push(&l->trace, start, 1, "Unknown character");
    }

    next(l);
  }

  t->start = start;
  t->offset = l->srcOffset - start;
  return;
eof:
  t->start = l->srcOffset;
  t->offset = 0;
}

void lexer_free(struct lexer* l) {
}
