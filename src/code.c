#include "code.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void emit_opcode(struct chunk* c, enum opcode code) {
  if (c->instructions.len >= c->instructions.cap) {
    c->instructions.cap *= 2;
    enum opcode* buf =
      (enum opcode*)malloc(sizeof(enum opcode) * c->instructions.cap);
    memcpy(buf, c->instructions.opCodes, sizeof(enum opcode) * c->instructions.len);
    free(c->instructions.opCodes);
    c->instructions.opCodes = buf;
  }

  c->instructions.opCodes[c->instructions.len++] = code;
}

static void emit_value(struct chunk* c, value_t v) {
  if (c->constants.len >= c->constants.cap) {
    c->constants.cap *= 2;
    value_t* buf =
      (value_t*)malloc(sizeof(value_t) * c->constants.cap);
    memcpy(buf, c->constants.values, sizeof(value_t) * c->constants.len);
    free(c->constants.values);
    c->constants.values = buf;
  }

  c->constants.values[c->constants.len++] = v;
}

void chunk_init(struct chunk* c) {
  c->instructions.len = 0;
  c->instructions.cap = 256;
  c->instructions.opCodes =
    (enum opcode*)malloc(sizeof(enum opcode) * c->instructions.cap);

  c->constants.len = 0;
  c->constants.cap = 256;
  c->constants.values =
    (value_t*)malloc(sizeof(value_t) * c->constants.cap);
}

void chunk_emit_constant(struct chunk* c, value_t constant) {
  emit_opcode(c, OP_LOAD_CONSTANT);
  emit_value(c, constant);
}

void chunk_emit_instruction(struct chunk* c, enum opcode code) {
  emit_opcode(c, code);
}

#define OPCODE_1(op)				\
  op:						\
  printf(#op);				\
  break;					\

void chunk_debug(struct chunk* c) {
  printf("=== CHUNK %p ===\n", c);
  int pc = 0, cc = 0;
  for (;;) {
    printf(" . %03d ", pc);
    enum opcode op = c->instructions.opCodes[pc];
    switch (op) {
    case OPCODE_1(OP_ADD);
    case OPCODE_1(OP_SUB);
    case OPCODE_1(OP_MUL);
    case OPCODE_1(OP_DIV);

    case OP_LOAD_CONSTANT:
      printf("OP_LOAD_CONSTANT");
      printf("\t%f", c->constants.values[cc++]);
      break;
    case OP_RET:
      printf("OP_RET");
      break;
    case OP_EOF:
      printf("OP_EOF\n");
      goto eof;
    }
    printf("\n");
    
    pc++;
  }
eof:
  printf("==============================\n");
}

void chunk_free(struct chunk* c) {
  free(c->instructions.opCodes);
  free(c->constants.values);
}

