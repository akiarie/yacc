#include <stdio.h>

#include "grammar.h"
#include "gen.h"
#include "util.h"

int
main()
{
	Grammar *G = grammar_create("line");
	map_set(G->map, "line", symbol_inline(
		prod_inline("expr", "\n")
	));
	map_set(G->map, "expr", symbol_inline(
		prod_inline("expr", "+", "term"),
		prod_inline("term")
	));
	map_set(G->map, "term", symbol_inline(
		prod_inline("term", "*", "factor"),
		prod_inline("factor")
	));
	map_set(G->map, "factor", symbol_inline(
		prod_inline("(", "expr", ")"),
		prod_inline("DIGIT")
	));
	gprintf("%j\n", G);
	gen(stdout, G);
	grammar_destroy(G);
}
