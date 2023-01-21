%{
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "util.h"
#include "grammar.h"
#include "lex.h"

static void
addtopreamble();

static void
setstart(char *sym);

static void
addnt();

static void
startnt();

static void
ntaddprod();

static void
startprod();

static void
prodaddsym();

static void
finish();
%}
/* grammar for the input to yacc
 * https://www.unix.com/man-page/POSIX/1posix/yacc */

/* basic entries */
/* the following are recognized by the lexical analyzer */
%token IDENTIFIER      /* includes identifiers and literals */
%token C_IDENTIFIER    /* identifier (but not literal) followed by a : */
%token NUMBER          /* [0-9][0-9]* */

/* reserved words : %type=>TYPE %left=>LEFT, and so on */
%token LEFT RIGHT NONASSOC TOKEN PREC TYPE START UNION
%token MARK            /* the %% mark */
%token LCURL           /* the %{ mark */
%token RCURL           /* the %} mark */

/* 8-bit character literals stand for themselves; */
/* tokens have to be defined for multi-byte characters */
%start spec
%%

spec	: defs MARK rules tail { finish(); }
	;
tail	: MARK { /* in this action, set up the rest of the file */ }
     	| /* empty: the second MARK is optional */
	;
defs	: /* empty */
	| defs def
	;
def	: START IDENTIFIER { assert(false); /* setstart($2); */ }
	| UNION { /* copy union definition to output */ assert(false); }
	| LCURL { addtopreamble(); } RCURL
	| rword tag nlist
	;
rword	: TOKEN
	| LEFT
      	| RIGHT
      	| NONASSOC
      	| TYPE
      	;
tag	: /* empty: union tag optional */
	| '<' IDENTIFIER '>'
      	;
nlist	: nmno
      	| nlist nmno
      	;
nmno	: IDENTIFIER         /* note: literal invalid with % type */
	| IDENTIFIER NUMBER  /* note: invalid with % type */
	;

/* rules section */
rules	: rules rule { addnt(); }
	| rule { addnt(); }
      	;
rule	: C_IDENTIFIER { startnt(); } prod
	| rule '|' prod
      	;
prod	: { startprod(); } rbody prec { ntaddprod(); }
     	;
rbody	: /* empty */
	| rbody IDENTIFIER { prodaddsym(); }
      	| rbody act
      	;
act	: '{' { /* copy action, translate $$, and so on */ } '}'
	;
prec	: /* empty */
	| PREC IDENTIFIER
      	| PREC IDENTIFIER act
      	| prec ';'
      	;
%%

static Prod *p = NULL;

static void
startprod()
{
	assert(!p);
	p = prod_create("");
}

static char *
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

static void
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

static char *symX = NULL;
static Nonterminal *X = NULL;

static void
ntaddprod()
{
	assert(X && p);
	nonterminal_addprod(X, p);
	p = NULL;
}

static void
startnt()
{
	assert(!symX && !X);
	symX = yylexeme();
	X = nonterminal_create();
}

static Grammar *G = NULL;

static void
setstart(char *sym)
{
	assert(!G);
	G = grammar_create(sym);
}

static void
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

static void
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

static void
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
