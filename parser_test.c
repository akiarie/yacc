#include <stdio.h>
#include <assert.h>

#include "grammar.h"
#include "parser.h"
#include "util.h"

void
S()
{
	Grammar *g = grammar_parse(
		"S : S S + | S S * | a;"
	);
	Grammar *G = grammar_augment(g);
	Parser P = parser_create(G);
	Symbolset *order = symbolset_create(
		"a", "+", "*", "$"
	);
	char *output = parser_str_ordered(P, order);
	prod_destroy(order);
	printf("%s\n", output);
	free(output);
	parser_destroy(P);
	grammar_destroy(G);
	grammar_destroy(g);
}

void
expr()
{
	Grammar *g = grammar_parse(
		"E : E + T | T;"
		"T : T * F | F;"
		"F : ( E ) | id;"
	);
	Grammar *G = grammar_augment(g);
	Parser P = parser_create(G);
	Symbolset *order = symbolset_create(
		"id", "+", "*", "(", ")", "$", "E", "T", "F"
	);
	char *output = parser_str_ordered(P, order);
	prod_destroy(order);
	printf("%s\n", output);
	free(output);
	parser_destroy(P);
	grammar_destroy(G);
	grammar_destroy(g);
}


int main()
{
	/*S();*/
	expr();
}
