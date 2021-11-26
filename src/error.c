#include "error.h"
#include <stdio.h>

void stacktrace_init(struct stacktrace* s) {
  s->last = 0;
  s->errorCount = 0;
}

void stacktrace_push(struct stacktrace* s, int start, int offset, char* errMsg) {
  if (s->errorCount >= STACKTRACE_LIMIT) {
    s->last = 0;
  }

  s->errorLocs[s->last].start  = start;
  s->errorLocs[s->last].offset = offset;
  s->errorMsgs[s->last] = errMsg;

  s->last++;
  s->errorCount++;
}

static void count_nl(char* src, int srcLen, int to,
		     int* outNls, int* outOffset, int* outSol, int* outEol) {
  int nls = 0, offs = 0, sol = 0;
  for (int i = 0; i < to; i++) {
    offs++;
    if (src[i] == '\n') {
      sol = i + 1;
      offs = 0;
      nls++;
    }
  }

  int eol = to;
  while (eol < srcLen) {
    if (src[eol] == '\n') break;
    eol++;
  }
  *outNls = nls;
  *outOffset = offs;
  *outEol = eol;
  *outSol = sol;
}

void putmany(char c, int n) {
  for (int i = 0; i < n; i++) {
    putchar(c);
  }
}

// Reports errors
// Example error:
// 0:0: error: Unknown character
//   'abc $$$ abc'
//        ~~~
void stacktrace_report(struct stacktrace* s, char* src, int srcLen) {
  int errorMax =
    (s->errorCount >= STACKTRACE_LIMIT) ? STACKTRACE_LIMIT : s->errorCount;
  for (int i = 0; i < errorMax; i++) {
    struct errorlocation errLoc = s->errorLocs[i];
    int line, lineOffset, eol, sol;
    count_nl(src, srcLen, errLoc.start, &line, &lineOffset, &sol, &eol);
    printf("%d:%d: ", line, lineOffset);
    printf("\033[0;31m"); // set terminal text red
    printf("error: ");
    printf("\033[0m"); // set terminal text back to normal
    printf("%s\n", s->errorMsgs[i]);
    
    // print line in which error occurs
    printf("  '");
    for (int j = sol; j < eol; j++) {
      putchar(src[j]);
    }
    printf("'\n");

    // indicate where error is
    // e.g "a $$$ b"
    //        ~~~
    // Unknown operator
    printf("\033[0;33m"); // set terminal text yellow
    // the + 3 is due to the 2 spaces of indentation and the ' character
    putmany(' ', lineOffset + 3);
    putmany('~', errLoc.offset);
    printf("\033[0m\n"); // set terminal text back to normal
  }
}
