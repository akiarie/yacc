#include <stdio.h>

#include "grammar.h"
#include "parser.h"
#include "gen.h"
#include "util.h"

int
main()
{
	Grammar *G = grammar_create("line");
	map_set(G->map, "line", nonterminal_inline(
		prod_inline("printf(\"%%d\\n\", $1);", "expr", "\\n")
	));
	map_set(G->map, "expr", nonterminal_inline(
		prod_inline("$$ = $1 + $3;", "expr", "+", "term"),
		prod_inline(NULL, "term")
	));
	map_set(G->map, "term", nonterminal_inline(
		prod_inline("$$ = $1 * $3;", "term", "*", "factor"),
		prod_inline(NULL, "factor")
	));
	map_set(G->map, "factor", nonterminal_inline(
		prod_inline("$$ = $2;", "(", "expr", ")"),
		prod_inline(NULL, "DIGIT")
	));
	Grammar *GG = grammar_augment(G);
	Parser P = parser_create(GG, 
/* preamble */
"#include <ctype.h>\n"
"\n"
"int yylval;\n",
/* postamble */
"\n"
"int\n"
"yylex()\n"
"{\n"
"	int c = getchar();\n"
"	if (isdigit(c)) {\n"
"		yylval = c - '0';\n"
"		return DIGIT;\n"
"	}\n"
"	return c;\n"
"}\n"
"\n"
"int\n"
"main()\n"
"{\n"
"	return yyparse();\n"
"}\n");
	gen(stdout, P);
	parser_destroy(P);
	grammar_destroy(G);
	grammar_destroy(GG);
}
