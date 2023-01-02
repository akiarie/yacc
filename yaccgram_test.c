#include <stdio.h>

#include "grammar.h"
#include "yaccgram.h"

int
main()
{
	Grammar *G = yaccgram();
	gprintf("%j\n", G);
	grammar_destroy(G);
}
