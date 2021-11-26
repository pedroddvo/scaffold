#ifndef VM_H
#define VM_H
#include "code.h"

struct vm {
  value_t stack[256];
  int top;

  int ic, cc; // instruction count, constant count

  int error;
  char* errorMsg;
};

void vm_execute(struct vm* vm, struct chunk* chunk);
value_t vm_top(struct vm* vm);
void vm_free(struct vm* vm);

#endif
