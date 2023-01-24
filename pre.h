#ifndef YY_PRE
#define YY_PRE

/* header inclusion justified because of repeated uses of this file */
#include <assert.h>
#include <stdbool.h>
#include "lex.h"

void
addtopreamble();

void
setstart(char *sym);

void
start();

void
addnt();

void
startnt();

void
ntaddprod();

void
startprod();

void
prodaddsym();

void
finish();

#endif
