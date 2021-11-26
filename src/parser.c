#include "parser.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char* token_repr(struct token* t) {
  switch (t->id) {
  case TOKEN_EOF:
    return "end of file";
  case TOKEN_ERROR:
    return "error";
  case TOKEN_SOF:
    return "start of file";
  case TOKEN_NUMBER:
    return "number";
  case TOKEN_SYMBOL:
    return "symbol";
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_SLASH:
  case TOKEN_STAR:
  case TOKEN_LPAREN:
  case TOKEN_RPAREN:
    return "operator";
  }
}

static void expect(struct parser* p, struct token* t,
		   const char* expected, const char* got, bool expr) {
  if (!expr) {
    char buf[100];
    sprintf(buf, "Expected %s, got %s", expected, got);
    stacktrace_push(&p->trace, t->start, t->offset, buf);
  }
}

static void checkeof(struct parser* p, struct token* t) {
  expect(p, t, "expression", "end of file",
	 (t->id != TOKEN_EOF));
}

static struct token* peek(struct parser* p) {
  return &p->tokenized.tokens[p->tokOffset];
}

static struct token* next(struct parser* p) {
  struct token* t = peek(p);
  checkeof(p, t); // check if over peeked into EOF
  return &p->tokenized.tokens[p->tokOffset++];
}

// free after using this function!!
static char* tokenstr_malloc(struct token* t, char* src) {
  char* tstr = (char*)malloc(t->offset + 1); // +1 null termination
  for (int i = 0; i < t->offset; i++) {
    tstr[i] = src[t->start + i];
  }
  tstr[t->offset] = '\0';
  return tstr;
}

static void emit_atomic(struct parser* p) {
  struct token* t = next(p);
  checkeof(p, t);
  switch (t->id) {
  case TOKEN_NUMBER:
    char* tstr = tokenstr_malloc(t, p->src);
    chunk_emit_constant(p->chunk, atof(tstr));
    free(tstr);
    break;
  case TOKEN_LPAREN:
    emit_expr(p);
    t = next(p);
    break;
    

  default:
    stacktrace_push(&p->trace, t->start, t->offset,
		    "Unexpected expression");
  }
}

// This splits two 16 bit (short) values into one integer
// This allows us to return a sort of tuple(short, short)
// It works like this, imagine you have an 8 bit number:
// 0000 0000 <- we can add the number 1 on the left side using 1 << 4
// 0001 0000 <- we can then retrieve the one using 1 >> 4
#define SPLIT_VALS(a, b) (a << 16) + (b)

static int infix_bp(struct token* t) {
  switch (t->id) {
  case TOKEN_PLUS:
  case TOKEN_MINUS:
    return SPLIT_VALS(10, 11);
  case TOKEN_STAR:
  case TOKEN_SLASH:
    return SPLIT_VALS(12, 13);

  default:
    return -1;
  }
}

static void emit_binary(struct parser* p, int minbp) {
  emit_atomic(p);

  for (;;) {
    struct token* op_t = peek(p);
    switch (op_t->id) {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_STAR:
    case TOKEN_SLASH:
    case TOKEN_LPAREN:
    case TOKEN_RPAREN:
      break;
    case TOKEN_EOF:
      return;
    default:
      stacktrace_push(&p->trace, op_t->start, op_t->offset,
		      "Unexpected token");
    }

#define CASE_TOKEN(tok, op) case tok: chunk_emit_instruction(p->chunk, op); break;
    
    int ibp = infix_bp(op_t);
    if (ibp != -1) {
      int lbp = ibp >> 16;
      if (lbp < minbp) break;
      int rbp = ibp - (lbp << 16);

      next(p);
      emit_binary(p, rbp);

      switch (op_t->id) {
	CASE_TOKEN(TOKEN_PLUS,  OP_ADD);
	CASE_TOKEN(TOKEN_MINUS, OP_SUB);
	CASE_TOKEN(TOKEN_STAR,  OP_MUL);
	CASE_TOKEN(TOKEN_SLASH, OP_DIV);
      default:
	stacktrace_push(&p->trace, op_t->start, op_t->offset,
			"Undefined opcode for infix operator");
      }
    }

    break;
  }
}

void emit_expr(struct parser* p) {
  emit_binary(p, 0);
}

void parse(char* src, int srcLen, struct chunk* emitted) {
  struct parser parser;
  parser.tokOffset = 0;
  parser.src = src;
  stacktrace_init(&parser.trace);
  struct lexer lexer;
  lexer_init(&lexer, src, srcLen);

  // start the lexing process...

  // lets make a list of all tokens dynamically
  struct tokenized t;
  t.len = 0;
  t.cap = 8; // maybe increase capacity to prevent copies?
  t.tokens = (struct token*)malloc(sizeof(struct token) * t.cap);

  for (;;) {
    struct token token;
    lexer_token(&lexer, &token);
    // grow tokens array
    if (t.len >= t.cap) {
      t.cap *= 2;
      struct token* buf =
	(struct token*)malloc(sizeof(struct token) * t.cap);
      memcpy(buf, t.tokens, sizeof(struct token) * t.len);
      free(t.tokens);
      t.tokens = buf;
    }

    // push the token into the array
    t.tokens[t.len++] = token;

    if (token.id == TOKEN_EOF) break;
  }

  parser.tokenized = t;

  // catch any unclosed parentheses
  if (lexer.parenCount > 0) {
    stacktrace_push(&lexer.trace, lexer.parenLast, 1,
		    "Unclosed parenthesis");
  }

  // if we catch any lexing errors...
  if (lexer.trace.errorCount > 0) {
    stacktrace_report(&lexer.trace, src, srcLen);
    goto cleanup;
  }

  // begin the parsing...
  chunk_init(emitted);
  parser.chunk = emitted;
  emit_expr(&parser);
  chunk_emit_instruction(parser.chunk, OP_EOF);

  if (parser.trace.errorCount > 0) {
    stacktrace_report(&parser.trace, src, srcLen);
    goto cleanup;
  }

cleanup:
  free(t.tokens);
  lexer_free(&lexer);
}
