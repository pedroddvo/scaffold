#ifndef ERROR_H
#define ERROR_H

#define STACKTRACE_LIMIT 8

struct errorlocation {
  int start;
  int offset;
};

struct stacktrace {
  char*  errorMsgs[STACKTRACE_LIMIT]; // NULL TERMINATED!!
  struct errorlocation errorLocs[STACKTRACE_LIMIT];

  int    errorCount;
  int    last;
};

void stacktrace_init(struct stacktrace* s);
void stacktrace_push(struct stacktrace* s, int start, int offset, char* errMsg);
void stacktrace_report(struct stacktrace* s, char* src, int srcLen);

#endif
