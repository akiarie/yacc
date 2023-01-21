#include <stdio.h>
#include <string.h>

#include "grammar.h"
#include "parser.h"
#include "gen.h"
#include "util.h"

#define EXAMPLE_FILE "gen_test_gen.c"

/* read_file: reads contents of file and returns them
 * caller must free returned string
 * see https://stackoverflow.com/a/14002993 */
char *
read_file(char *path)
{
	FILE *f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  /* same as rewind(f); */
	char *str = malloc(fsize + 1);
	fread(str, fsize, 1, f);
	fclose(f);
	str[fsize] = '\0';
	return str;
}

int
main()
{
	Grammar *G = grammar_create("line");
	map_set(G->map, "line", nonterminal_inline(
		prod_inline("printf(\"%d\\n\", $1);", "expr", "\\n")
	));
	map_set(G->map, "expr", nonterminal_inline(
		prod_inline("$$ = $1 + $3;", "expr", "+", "term"),
		prod_inline("", "term")
	));
	map_set(G->map, "term", nonterminal_inline(
		prod_inline("$$ = $1 * $3;", "term", "*", "factor"),
		prod_inline("", "factor")
	));
	map_set(G->map, "factor", nonterminal_inline(
		prod_inline("$$ = $2;", "(", "expr", ")"),
		prod_inline("", "DIGIT")
	));
	printf("%s\n", grammar_str(G));
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
"}\n");
	char *buf = NULL;
	size_t buflen = 0;
	FILE *stream = open_memstream(&buf, &buflen);
	gen(stream, P);
	fclose(stream);
	char *expected = read_file(EXAMPLE_FILE);
	if (strcmp(buf, expected) != 0) {
		fprintf(stderr, "generated file does not match expected\n");
		exit(1);
	}
	free(expected);
	free(buf);
	parser_destroy(P);
	grammar_destroy(GG);
	grammar_destroy(G);
}
