#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "main.h"

#define TK_EOF 0
#define TK_ERROR -1

static char *tokstr[] = {
	/* basic entities */
	[IDENTIFIER]	= "identifier",
	[C_IDENTIFIER]	= "c_identifier",
	[NUMBER]	= "number",

	/* reserved words: %type => TYPE, %left => LEFT, etc. */
	[LEFT]		= "%" "left",
	[RIGHT]		= "%" "right",
	[NONASSOC]	= "%" "nonassoc",
	[TOKEN]		= "%" "token",
	[PREC]		= "%" "prec",
	[TYPE]		= "%" "type",
	[START]		= "%" "start",
	[UNION] 	= "%" "union",

	/* punctuation */
	[MARK]		= "mark",	/* %% */
	[LCURL] 	= "lcurl",	/* %{ */
	[RCURL] 	= "rcurl",	/* %} */
};

#define LEN(a) (sizeof(a) / sizeof((a)[0]))

typedef struct {
	int type;
	char *lexeme;
	size_t len;
} Token;

static const Token tkError = {TK_ERROR, NULL, 0};

/* skipws: skip whitespace */
static char *
skipws(char *s)
{
	for (; isspace(*s); s++) {}
	return s;
}

static Token
lex_punct(char *s)
{
	assert(false);
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
		if (strncmp(s, kw, strlen(kw)) == 0) {
			return (Token) {
				.type	= keyword[i],
				.lexeme	= (char *) kw,
			};
		}
	}
	return tkError;
}

static Token
lex_id(char *start)
{
	assert(false);
}

static Token
lex_num(char *start)
{
	assert(false);
}

static Token
parse_next(char *pos)
{
	typedef Token (*token_lexer)(char *);
	token_lexer lexers[] = {
		lex_punct,
		lex_keyword,
		lex_id,
		lex_num,
	};
	for (int i = 0; i < LEN(lexers); i++) {
		Token tk = lexers[i](pos);
		if (tk.type != TK_ERROR) {
			return tk;
		}
	}
	fprintf(stderr, "cannot parse token near '%.*s'\n", 10, pos);
	exit(EXIT_FAILURE);
}

/* lex: return array of tokens ending with an EOF token */
static Token *
lex(char *pos)
{
	size_t len = 0;
	Token *tokens = NULL;
	for (Token tk = tkError; tk.type != TK_EOF;
			tokens[len++] = tk) {
		pos = skipws(pos);
		tk = parse_next(skipws(pos));
		pos += tk.len;
		tokens = (Token *) realloc(tokens, sizeof(Token) * (len + 1));
	}
	return tokens;
}
