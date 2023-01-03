#include <stdio.h>

#include "grammar.h"
#include "parser.h"
#include "gen.h"
#include "util.h"

int main()
{
	Grammar *G = grammar_create("line");
	map_set(G->map, "line", symbol_inline(
		prod_inline("printf(\"%%d\\n\", $1);", "expr", "\n")
	));
	map_set(G->map, "expr", symbol_inline(
		prod_inline("$$ = $1 + $3;", "expr", "+", "term"),
		prod_inline(NULL, "term")
	));
	map_set(G->map, "term", symbol_inline(
		prod_inline("$$ = $1 * $3;", "term", "*", "factor"),
		prod_inline(NULL, "factor")
	));
	map_set(G->map, "factor", symbol_inline(
		prod_inline("$$ = $2;", "(", "expr", ")"),
		prod_inline(NULL, "id")
	));
	Grammar *GG = grammar_augment(G);
	Parser P = parser_create(GG);
	Symbolset *order = symbolset_create(
		"id", "+", "*", "(", ")", "\n", "$", "line", "expr", "term", "factor"
	);
	char *output = parser_str_ordered(P, order);
	prod_destroy(order);
	printf("%s\n", output);
	free(output);
	gen(stdout, P);
	parser_destroy(P);
	grammar_destroy(G);
	grammar_destroy(GG);
}
