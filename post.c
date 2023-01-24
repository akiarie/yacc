#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "grammar.h"
#include "util.h"

#include "pre.h"

Prod *p = NULL;

void
startprod()
{
	assert(!p);
	p = prod_create("");
}

char *
getliteral(char *s)
{
	struct strbuilder *b = strbuilder_create();
	switch (*++s) {
	case '\\': /* escape */
		strbuilder_printf(b, "\\%c", s[1]);
		break;
	default:
		strbuilder_putc(b, s[0]);
		break;
	}
	return strbuilder_build(b);
}

void
prodaddsym()
{
	char *raw = yylexeme();
	char *sym;
	switch (raw[0]) {
	case '\'':
		sym = getliteral(raw);
		break;
	default:
		sym = raw;
		break;
	}
	prod_append(p, sym);
}

char *symX = NULL;
Nonterminal *X = NULL;

void
ntaddprod()
{
	assert(X && p);
	nonterminal_addprod(X, p);
	p = NULL;
}

void
startnt()
{
	assert(!symX && !X);
	symX = yylexeme();
	X = nonterminal_create();
}

Grammar *G = NULL;

void
setstart(char *sym)
{
	assert(!G);
	G = grammar_create(sym);
}

void
start()
{
	setstart(yylexeme());
}

void
addnt()
{
	assert(symX && X);
	if (!G) {
		setstart(symX);
	}
	map_set(G->map, symX, X);
	X = NULL;
	symX = NULL;
}

struct strbuilder *bpre = NULL;

void
addtopreamble()
{
	if (!bpre) {
		bpre = strbuilder_create();
	}
	char *s = yytext();
	assert(strlen(s) > 2);
	strbuilder_printf(bpre, "%s\n", s+2); /* skip %{ */
	free(s);
}

void
finish()
{
	printf("%s\n", grammar_str(G));
}

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

#define DEFAULT_FILE "y.tab.c"

int
yyparse();

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "must include grammar\n");
		exit(EXIT_FAILURE);
	}
	char *file = read_file(argv[1]);
	yyscanstring(file);
	yyparse();
	/* TODO: print G */
	free(file);
}
