#include <stdio.h>

#include "grammar.h"
#include "parser.h"
#include "gen.h"
#include "util.h"

int
main()
{
	/* TODO:
	 * - write lexer
	 * - use lexeme location in string to access data for actions
	 * - add production actions to grammar (middle actions treated as empty
	 *   productions)
	 * - think through translation of $$ etc.
	 * - write checker for terminals to match those declared
	 * - precedence etc.
	 */
	Grammar *G = grammar_create("spec");
	map_set(G->map, "spec", nonterminal_inline(
		prod_inline("defs", "MARK", "rules", "tail")
	));
	map_set(G->map, "tail", nonterminal_inline(
		prod_inline(NULL, "MARK"),
		prod_create() /* ε */
	));
	map_set(G->map, "defs", nonterminal_inline(
		prod_create(), /* ε */
		prod_inline("defs", "def")
	));
	map_set(G->map, "def", nonterminal_inline(
		prod_inline(NULL, "START", "IDENTIFIER"),
		prod_inline(NULL, "UNION"),
		prod_inline(NULL, "LCURL", "RCURL"),
		prod_inline(NULL, "rword", "tag", "nlist")
	));
	map_set(G->map, "rword", nonterminal_inline(
		prod_inline(NULL, "TOKEN"),
		prod_inline(NULL, "LEFT"),
		prod_inline(NULL, "RIGHT"),
		prod_inline(NULL, "NONASSOC"),
		prod_inline(NULL, "TYPE")
	));
	map_set(G->map, "tag", nonterminal_inline(
		prod_create(), /* ε */
		prod_inline(NULL, "<", "IDENTIFIER", ">")
	));
	map_set(G->map, "nlist", nonterminal_inline(
		prod_inline(NULL, "nmno"),
		prod_inline(NULL, "nlist", "nmno")
	));
	map_set(G->map, "nmno", nonterminal_inline(
		prod_inline(NULL, "IDENTIFIER"),
		prod_inline(NULL, "IDENTIFIER", "NUMBER")
	));
	/* rules section */
	map_set(G->map, "rules", nonterminal_inline(
		prod_inline(NULL, "C_IDENTIFIER", "rbody", "prec"),
		prod_inline(NULL, "rules", "rule")
	));
	map_set(G->map, "rule", nonterminal_inline(
		prod_inline(NULL, "C_IDENTIFIER", "rbody", "prec"),
		prod_inline(NULL, "|", "rbody", "prec")
	));
	map_set(G->map, "rbody", nonterminal_inline(
		prod_create(), /* ε */
		prod_inline(NULL, "rbody", "IDENTIFIER"),
		prod_inline(NULL, "rbody", "act")
	));
	map_set(G->map, "act", nonterminal_inline(
		prod_inline(NULL, "{", "}")
	));
	map_set(G->map, "prec", nonterminal_inline(
		prod_create(), /* ε */
		prod_inline(NULL, "PREC", "IDENTIFIER"),
		prod_inline(NULL, "PREC", "IDENTIFIER", "act"),
		prod_inline(NULL, "prec", ";")
	));
	Grammar *GG = grammar_augment(G);
	Parser P = parser_create(GG, "", "");
	gen(stdout, P);
	parser_destroy(P);
	grammar_destroy(GG);
	grammar_destroy(G);
}
