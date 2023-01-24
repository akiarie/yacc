#include <stdio.h>

#include "grammar.h"
#include "parser.h"
#include "gen.h"
#include "util.h"

static void
genfiles(FILE *codefile, FILE *headerfile)
{
	Grammar *G = grammar_create("spec");
	map_set(G->map, "spec", nonterminal_inline(
		prod_inline("", "defs", "MARK", "rules", "tail", "finish_")
	));
	map_set(G->map, "tail", nonterminal_inline(
		prod_inline("", "MARK"),
		prod_epsilon() /* ε */
	));
	map_set(G->map, "defs", nonterminal_inline(
		prod_epsilon(), /* ε */
		prod_inline("", "defs", "def")
	));
	map_set(G->map, "def", nonterminal_inline(
		prod_inline("", "START", "IDENTIFIER", "start_"),
		prod_inline("", "UNION"),
		prod_inline("", "LCURL", "paddto_", "RCURL"),
		prod_inline("", "rword", "tag", "nlist")
	));
	map_set(G->map, "rword", nonterminal_inline(
		prod_inline("", "TOKEN"),
		prod_inline("", "LEFT"),
		prod_inline("", "RIGHT"),
		prod_inline("", "NONASSOC"),
		prod_inline("", "TYPE")
	));
	map_set(G->map, "tag", nonterminal_inline(
		prod_epsilon(), /* ε */
		prod_inline("", "<", "IDENTIFIER", ">")
	));
	map_set(G->map, "nlist", nonterminal_inline(
		prod_inline("", "nmno"),
		prod_inline("", "nlist", "nmno")
	));
	map_set(G->map, "nmno", nonterminal_inline(
		prod_inline("", "IDENTIFIER"),
		prod_inline("", "IDENTIFIER", "NUMBER")
	));
	/* rules section */
	map_set(G->map, "rules", nonterminal_inline(
		prod_inline("", "rules", "rule", "addnt_"),
		prod_inline("", "rule", "addnt_")
	));
	map_set(G->map, "rule", nonterminal_inline(
		prod_inline("", "C_IDENTIFIER", "stnt_", "prod"),
		prod_inline("", "rule", "|", "prod")
	));
	map_set(G->map, "prod", nonterminal_inline(
		prod_inline("", "stprod_", "rbody", "prec", "ntadd_")
	));
	map_set(G->map, "rbody", nonterminal_inline(
		prod_epsilon(), /* ε */
		prod_inline("", "rbody", "IDENTIFIER", "pradd_"),
		prod_inline("", "rbody", "act")
	));
	map_set(G->map, "act", nonterminal_inline(
		prod_inline("", "{", "}")
	));
	map_set(G->map, "prec", nonterminal_inline(
		prod_epsilon(), /* ε */
		prod_inline("", "PREC", "IDENTIFIER"),
		prod_inline("", "PREC", "IDENTIFIER", "act"),
		prod_inline("", "prec", ";")
	));
	map_set(G->map, "finish_", nonterminal_inline(
		prod_bareact("finish();")
	));
	map_set(G->map, "start_", nonterminal_inline(
		prod_bareact("start();")
	));
	map_set(G->map, "paddto_", nonterminal_inline(
		prod_bareact("addtopreamble();")
	));
	map_set(G->map, "addnt_", nonterminal_inline(
		prod_bareact("addnt();")
	));
	map_set(G->map, "stnt_", nonterminal_inline(
		prod_bareact("startnt();")
	));
	map_set(G->map, "stprod_", nonterminal_inline(
		prod_bareact("startprod;")
	));
	map_set(G->map, "ntadd_", nonterminal_inline(
		prod_bareact("ntaddprod();")
	));
	map_set(G->map, "pradd_", nonterminal_inline(
		prod_bareact("prodaddsym();")
	));
	Grammar *GG = grammar_augment(G);
	Parser P = parser_create(GG,
"#include \"pre.h\"\n"
"\n",
"#include \"post.c\"\n"
"\n");
	gen(codefile, P);
	gen_headers(headerfile, P);
	parser_destroy(P);
	grammar_destroy(GG);
	grammar_destroy(G);
}

int
main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr,
"must provide code and header files\n"
"\n"
"Usage: %s code_file header_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	char *code = argv[1],
	     *headers = argv[2];
	FILE *fcode = fopen(code, "w"),
	     *fheaders = fopen(headers, "w");
	if (!fcode) {
		fprintf(stderr, "cannot write to code file\n");
		exit(EXIT_FAILURE);
	}
	if (!fheaders) {
		fprintf(stderr, "cannot write to headers file\n");
		exit(EXIT_FAILURE);
	}
	genfiles(fcode, fheaders);
	fclose(fheaders);
	fclose(fcode);
}
