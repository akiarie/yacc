#include "grammar.h"
#include "util.h"

Grammar *
yaccgram()
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
	map_set(G->map, "spec", symbol_inline(
		prod_inline("defs", "MARK", "rules", "tail")
	));
	map_set(G->map, "tail", symbol_inline(
		prod_inline("MARK"),
		prod_create() /* ε */
	));
	map_set(G->map, "defs", symbol_inline(
		prod_create(), /* ε */
		prod_inline("defs", "def")
	));
	map_set(G->map, "def", symbol_inline(
		prod_inline("START", "IDENTIFIER"),
		prod_inline("UNION"),
		prod_inline("LCURL", "RCURL"),
		prod_inline("rword", "tag", "nlist")
	));
	map_set(G->map, "rword", symbol_inline(
		prod_inline("TOKEN"),
		prod_inline("LEFT"),
		prod_inline("RIGHT"),
		prod_inline("NONASSOC"),
		prod_inline("TYPE")
	));
	map_set(G->map, "tag", symbol_inline(
		prod_create(), /* ε */
		prod_inline("<", "IDENTIFIER", ">")
	));
	map_set(G->map, "nlist", symbol_inline(
		prod_inline("nmno"),
		prod_inline("nlist", "nmno")
	));
	map_set(G->map, "nmno", symbol_inline(
		prod_inline("IDENTIFIER"),
		prod_inline("IDENTIFIER", "NUMBER")
	));
	/* rules section */
	map_set(G->map, "rules", symbol_inline(
		prod_inline("C_IDENTIFIER", "rbody", "prec"),
		prod_inline("rules", "rule")
	));
	map_set(G->map, "rule", symbol_inline(
		prod_inline("C_IDENTIFIER", "rbody", "prec"),
		prod_inline("|", "rbody", "prec")
	));
	map_set(G->map, "rbody", symbol_inline(
		prod_create(), /* ε */
		prod_inline("rbody", "IDENTIFIER"),
		prod_inline("rbody", "act")
	));
	map_set(G->map, "act", symbol_inline(
		prod_inline("{", "}")
	));
	map_set(G->map, "prec", symbol_inline(
		prod_create(), /* ε */
		prod_inline("PREC", "IDENTIFIER"),
		prod_inline("PREC", "IDENTIFIER", "act"),
		prod_inline("prec", ";")
	));
	return G;
}
