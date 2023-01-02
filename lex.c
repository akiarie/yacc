#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "lex.h"

static char *
tktype_str(enum tktype t)
{
	switch (t) {
	case TK_EOF:
		return "eof";
	case TK_IDENTIFIER:
		return "identifier";
	case TK_C_IDENTIFIER:
		return "c_identifier";
	case TK_NUMBER:
		return "number";
	case TK_LEFT:
		return "left";
	case TK_RIGHT:
		return "right";
	case TK_NONASSOC:
		return "nonassoc";
	case TK_TOKEN:
		return "token";
	case TK_PREC:
		return "prec";
	case TK_TYPE:
		return "type";
	case TK_START:
		return "start";
	case TK_UNION:
		return "union";
	case TK_MARK:
		return "mark";
	case TK_LCURL:
		return "lcurl";
	case TK_RCURL:
		return "rcurl";
	case TK_ERROR:
		return "error";
	default:
		fprintf(stderr, "unknown token %d\n", t);
		exit(EXIT_FAILURE);
	}
}

typedef struct {
	enum tktype type;
	char *value;
	size_t len;
} Token;

static const Token tkError = {TK_ERROR, NULL, 0};

static void
token_cleanup(Token *tk)
{
	for (Token *t = tk; t->type != TK_EOF; t++) {
		if (t->value != NULL) {
			free(t->value);
		}
	}
	free(tk);
}

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
	switch (*s) {
		/*case ':':*/
			/*return (Token) {TOKEN_ARROW, NULL, 1};*/
		/*case '|':*/
			/*return (Token) {TOKEN_BAR, NULL, 1};*/
		/*case ';':*/
			/*return (Token) {TOKEN_SEMICOLON, NULL, 1};*/
		/*case '\0':*/
			/*return (Token) {TK_EOF, NULL, 0};*/
		default:
			return tkError;
	}
}

/*static bool*/
/*issymbolchar(char c)*/
/*{*/
	/*return !isspace(c) && c != ';' && c != '\0';*/
/*}*/

/*static Token*/
/*lex_symbol(char *start)*/
/*{*/
	/*char *pos = start;*/
	/*for (; issymbolchar(*pos); pos++) {}*/
	/*int len = (pos - start) + 1;*/
	/*char *s = malloc(sizeof(char) * len);*/
	/*snprintf(s, len, "%s", start);*/
	/*return (Token) {TOKEN_SYMBOL, s, strlen(s)};*/
/*}*/

#define LEN(a) (sizeof(a) / sizeof((a)[0]))

static Token
parse_next(char *pos)
{
	typedef Token (*token_lexer)(char *);
	token_lexer lexers[] = {
		lex_punct,
		/*lex_symbol,*/
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
