#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "util.h"
#include "yacc.h"
#include "lex.h"

static char *tokstr[] = {
	/* basic entities */
	[IDENTIFIER]	= "identifier",
	[C_IDENTIFIER]	= "c_identifier",
	[NUMBER]	= "number",

	/* reserved words: %type => TYPE, %left => LEFT, etc. */
	[LEFT]		= "left",
	[RIGHT]		= "right",
	[NONASSOC]	= "nonassoc",
	[TOKEN]		= "token",
	[PREC]		= "prec",
	[TYPE]		= "type",
	[START]		= "start",
	[UNION] 	= "union",

	/* punctuation */
	[MARK]		= "mark",	/* %% */
	[LCURL] 	= "lcurl",	/* %{ */
	[RCURL] 	= "rcurl",	/* %} */
};

static char *
char_str(char c)
{
	char *s = malloc(sizeof(char) * 2);
	snprintf(s, 2, "%c", c);
	return s;
}

char *
yytokstr(int tk)
{
	switch (tk) {
	case TK_EOF:
		return "eof";
	case TK_ERROR:
		return "error";
	default:
		if (tk < 257) {
			return char_str(tk);
		}
		return tokstr[tk];
	}
}

#define LEN(a) (sizeof(a) / sizeof((a)[0]))

typedef struct {
	int type;
	char *lexeme;
	size_t len;
} Token;

static const Token tkError = {TK_ERROR, ""};
static const Token tkEof = {TK_EOF, ""};

/* skipws: skip whitespace */
static char *
skipws(char *s)
{
	for (; isspace(*s); s++) {}
	return s;
}

static char *
skipwscomments(char *s)
{
	s = skipws(s);
	if (strncmp(s, "/*", 2) == 0) {
		while (strncmp(s++, "*/", 2) != 0) {}
		s++; /* ended with *s == '/' */
	}
	return isspace(*s) ? skipwscomments(s) : s;
}

static int
toseq(char *s, char *seq)
{
	char *loc = strstr(s, seq);
	if (!loc) {
		return -1;
	}
	return loc - s;
}

static int marks = 0;

static Token
lex_punct(char *s)
{
	char *loc = NULL;
	switch (*s) {
	case '<':
		return (Token) { '<', "<", 1 };
	case '>':
		return (Token) { '>', ">", 1 };
	case '{':
		loc = strstr(++s, "}");
		assert(loc);
		return (Token) { '{', "{", 1 + loc - s };
	case '}':
		return (Token) { '}', "}", 1 };
	case '|':
		return (Token) { '|', "|", 1 };
	case ';':
		return (Token) { ';', ";", 1 };
	}
	if (*s++ != '%') {
		return tkError;
	}
	switch (*s) {
	case '%':
		marks++;
		return (Token) { MARK, "%%" , (marks > 1) ? 1 + strlen(s) : 2};
	case '{':
		loc = strstr(++s, "%}");
		assert(loc);
		return (Token) { LCURL, "%{", 2 + loc - s };
	case '}':
		return (Token) { RCURL, "%}", 2 };
	}
	return tkError;
}

static char *
substr(char *start, size_t len)
{
	size_t tlen = len + 1;
	char *s = malloc(sizeof(char) * tlen);
	snprintf(s, tlen, "%s", start);
	return s;
}

static Token
lex_keyword(char *start)
{
	char *s = start;
	if (*s++ != '%') {
		return tkError;
	}
	const int keyword[] = {
		LEFT, RIGHT, NONASSOC, TOKEN, PREC, TYPE, START, UNION,
	};
	size_t len = LEN(keyword);
	for (int i = 0; i < len; i++) {
		const char *kw = tokstr[keyword[i]];
		size_t kwlen = strlen(kw);
		if (strncmp(s, kw, kwlen) == 0) {
			return (Token) {
				.type	= keyword[i],
				.lexeme	= substr(start, kwlen + 1),
				.len	= kwlen + 1, /* increment above */
			};
		}
	}
	return tkError;
}

static Token
lex_literal(char *s)
{
	size_t len = 3;
	if (s[1] == '\\') {
		/* escape literal (no octal, hex for now) */
		assert(strchr("ntvbrfa\\?\'\"", s[2]));
		len = 4;
	}
	return (Token) { IDENTIFIER, substr(s, len), len };
}

static Token
lex_id(char *start)
{
	char *s = start;
	if (*s == '\'') {
		return lex_literal(start);
	}
	if (!isalpha(*s) && *s != '_') {
		return tkError;
	}
	while (isalnum(*s) || *s == '_') {
		s++;
	}
	size_t len = s - start;
	char *next = skipws(s);
	if (*next == ':') {
		return (Token) {
			C_IDENTIFIER,
			substr(start, len), 
			1 + next - start /* include the colon */
		};
	}
	return (Token) { IDENTIFIER, substr(start, len), len };
}

static Token
lex_num(char *start)
{
	char *s = start;
	while (isdigit(*s)) {
		s++;
	}
	size_t len = s - start;
	if (len == 0) { /* no number */
		return tkError;
	}
	return (Token) { NUMBER, substr(start, len), len };
}

static Token
parse_next(char *pos)
{
	typedef Token (*token_lexer)(char *);
	token_lexer lexers[] = { lex_punct, lex_keyword, lex_id, lex_num };
	char *s = skipwscomments(pos);
	if (*s == '\0') {
		return tkEof;
	}
	for (int i = 0; i < LEN(lexers); i++) {
		Token tk = lexers[i](s);
		if (tk.type != TK_ERROR) {
			tk.len += (s - pos);
			assert(tk.len > 0);
			return tk;
		}
	}
	fprintf(stderr, "cannot parse token near '%.*s'\n", 10, pos);
	exit(EXIT_FAILURE);
}

static char *yyinputstring = NULL,
	    *yypos = NULL;
static Token yylast = {TK_ERROR, ""},
	     yy2last = {TK_ERROR, ""};

int
yylex()
{
	if (!yyinputstring || !yypos) {
		fprintf(stderr, "null input\n");
		exit(EXIT_FAILURE);
	}
	Token tk = parse_next(yypos);
	yy2last = yylast;
	yylast = tk;
	yypos += tk.len;
	yylval = tk.type;
	return tk.type;
}

char *
yytext()
{
	assert(yylast.type != TK_ERROR);
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "%.*s", yy2last.len, yypos - yy2last.len);
	return strbuilder_build(b);
}

char *
yylexeme()
{
	return yy2last.lexeme;
}

void
yyscanstring(char *input)
{
	yyinputstring = yypos = input;
}

struct lexpos {
	int nlines, linenum, linepos;
	char *prevline, *thisline, *nextline;
};

struct lexpos
findlexpos()
{
	int nlines = 1, /* one-indexed */
	    linenum = 0,
	    linepos = -1;
	char *linestart = NULL;
	for (char *s = yyinputstring; *s != '\0'; s++) {
		if (*s == '\n') {
			linestart = s;
			nlines++;
		}
		if (s == yypos) {
			linenum = nlines;
			linepos = yypos - linestart;
		}
	}
	assert(linenum > 0 && linepos >= 0);
	return (struct lexpos) {
		nlines, linenum, linepos,
	};
}

static long
min(long a, long b)
{
	return a < b ? a : b;
}

static long
max(long a, long b)
{
	return a > b ? a : b;
}

static char *
tostartline(char *s)
{
	for (; s != yyinputstring && *s != '\n'; s--) {}
	return s + 1;
}

static char *
toendline(char *s)
{
	for (; *s != '\0' && *s != '\n'; s++) {}
	return s;
}

#define COLOUR_RED	"\x1B[31m"
#define COLOUR_GRN	"\x1B[32m"
#define COLOUR_YEL	"\x1B[33m"
#define COLOUR_BLU	"\x1B[34m"
#define COLOUR_MAG	"\x1B[35m"
#define COLOUR_CYN	"\x1B[36m"
#define COLOUR_WHT	"\x1B[37m"
#define COLOUR_OFF	"\x1B[0m"

static void
fprintf_context(char *s)
{
	char *start = tostartline(s);
	if (start > yyinputstring) {
		start = tostartline(start - 1);
	}
	char *end = toendline(s);
	fprintf(stderr, "%s", COLOUR_RED);
	fprintf(stderr, "%.*s\n", (int) (end - start), start);
	fprintf(stderr, "%s", COLOUR_OFF);
	for (char *t = (char *) min((long) s, (long) tostartline(s));
			t != s; t++) {
		fprintf(stderr, " ");
	}
	fprintf(stderr, "^\n");
}

void
yyerror(char *s)
{
	fflush(stdout);
	fprintf(stderr, "*** %s:\n", s);
	if (yylast.type != TK_ERROR) {
		yypos -= yylast.len;
	}
	fprintf_context(yypos);
	struct lexpos pos = findlexpos();
	fprintf(stderr, "on '%s' near position %d, line %d of %d.\n",
		yylast.lexeme, pos.linepos, pos.linenum, pos.nlines);
}
