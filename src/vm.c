#include "vm.h"

static void push_value(struct vm* vm, value_t v) {
  vm->stack[vm->top++] = v;
}

static void push_constant(struct vm* vm, struct chunk* chunk) {
  push_value(vm, chunk->constants.values[vm->cc++]);
}

static value_t pop_value(struct vm* vm) {
  return vm->stack[--vm->top];
}

static int fetch_op(struct vm* vm, struct chunk* chunk) {
  return chunk->instructions.opCodes[vm->ic++];
}

value_t vm_top(struct vm* vm) {
  return vm->stack[vm->top - 1];
}

void vm_execute(struct vm* vm, struct chunk* chunk) {
  vm->ic = 0;
  vm->cc = 0;
  vm->top = 0;
  vm->error = 0;

  for (;;) {
    switch (fetch_op(vm, chunk)) {
    case OP_LOAD_CONSTANT:
      push_constant(vm, chunk);
      break;
    case OP_ADD: {
      value_t a = pop_value(vm);
      value_t b = pop_value(vm);
      push_value(vm, b + a);
      break;
    }
    case OP_SUB: {
      value_t a = pop_value(vm);
      value_t b = pop_value(vm);
      push_value(vm, b - a);
      break;
    }
    case OP_MUL: {
      value_t a = pop_value(vm);
      value_t b = pop_value(vm);
      push_value(vm, b * a);
      break;
    }
    case OP_DIV: {
      value_t a = pop_value(vm);
      value_t b = pop_value(vm);
      push_value(vm, b / a);
      break;
    }
      
    case OP_EOF:
      return;
    }
  }
}

void vm_free(struct vm* vm) {
}
