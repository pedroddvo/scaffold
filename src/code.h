#ifndef CODE_H
#define CODE_H

typedef float value_t;

enum opcode {
  OP_EOF,
  OP_RET,
  OP_LOAD_CONSTANT,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
};

struct chunk {
  struct {
    int len;
    int cap;
    enum opcode* opCodes;
  } instructions;

  struct {
    int len;
    int cap;
    value_t* values;
  } constants;
};

void chunk_init(struct chunk* c);
void chunk_emit_constant(struct chunk* c, value_t constant);
void chunk_emit_instruction(struct chunk* c, enum opcode code);
void chunk_debug(struct chunk* c);
void chunk_free(struct chunk* c);
#endif
