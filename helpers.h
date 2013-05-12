#ifndef _HELPERS_H
#define _HELPERS_H

#define MAXLEN            256
#define MIN(A, B)         ((A) < (B) ? (A) : (B))

void warn(char *, ...);
__attribute__((noreturn))
void err(char *, ...);

#endif
