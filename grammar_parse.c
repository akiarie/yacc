#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "grammar.h"
#include "grammar_util.c"

#ifndef BNF_GRAMMAR_PARSE
#define BNF_GRAMMAR_PARSE

enum tktype {
	TOKEN_SYMBOL	= 1 << 0,
	TOKEN_ARROW	= 1 << 1,
	TOKEN_BAR	= 1 << 2,
	TOKEN_SEMICOLON	= 1 << 3,
	TOKEN_EOF	= 1 << 4,
	TOKEN_ERROR	= 1 << 5,
};

static char *
tktype_str(enum tktype type)
{
	switch (type) {
		case TOKEN_SYMBOL:
			return "symbol";
		case TOKEN_ARROW:
			return "arrow";
		case TOKEN_BAR:
			return "bar";
		case TOKEN_SEMICOLON:
			return "semicolon";
		case TOKEN_EOF:
			return "eof";
		case TOKEN_ERROR:
			return "error";
		default:
			return "unknown";
	}
}

typedef struct {
	enum tktype type;
	char *value;
	size_t len;
} Token;

static const Token tkError = {TOKEN_ERROR, NULL, 0};

static void
token_cleanup(Token *tk)
{
	for (Token *t = tk; t->type != TOKEN_EOF; t++) {
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
		case ':':
			return (Token) {TOKEN_ARROW, NULL, 1};
		case '|':
			return (Token) {TOKEN_BAR, NULL, 1};
		case ';':
			return (Token) {TOKEN_SEMICOLON, NULL, 1};
		case '\0':
			return (Token) {TOKEN_EOF, NULL, 0};
		default:
			return tkError;
	}
}

static bool
issymbolchar(char c)
{
	return !isspace(c) && c != ';' && c != '\0';
}

static Token
lex_symbol(char *start)
{
	char *pos = start;
	for (; issymbolchar(*pos); pos++) {}
	int len = (pos - start) + 1;
	char *s = malloc(sizeof(char) * len);
	snprintf(s, len, "%s", start);
	return (Token) {TOKEN_SYMBOL, s, strlen(s)};
}

#define LEN(a) (sizeof(a) / sizeof((a)[0]))

static Token
parse_next(char *pos)
{
	typedef Token (*token_lexer)(char *);
	token_lexer lexers[] = {
		lex_punct,
		lex_symbol,
	};
	for (int i = 0; i < LEN(lexers); i++) {
		Token tk = lexers[i](pos);
		if (tk.type != TOKEN_ERROR) {
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
	for (Token tk = tkError; tk.type != TOKEN_EOF;
			tokens[len++] = tk) {
		pos = skipws(pos);
		tk = parse_next(skipws(pos));
		pos += tk.len;
		tokens = (Token *) realloc(tokens, sizeof(Token) * (len + 1));
	}
	return tokens;
}

static Nonterminal *
map_getorset(struct map *map, char *key)
{
	if (!map_get(map, key)) {
		map_set(map, key, nonterminal_create());
	}
	return map_get(map, key);
}

static size_t
nonterminal_parseprod(Nonterminal *X, Token *start)
{
	Token *tk = start;
	if (tk->type != TOKEN_SYMBOL) {
		fprintf(stderr, "production begins with %s\n",
			tktype_str(tk->type));
		exit(EXIT_FAILURE);
	}
	Prod *p = prod_create(NULL);
	for (; tk->type == TOKEN_SYMBOL; tk++) {
		if (strcmp(tk->value, SYMBOL_EPSILON) == 0) {
			/* ε must be alone */
			assert((tk - 1)->type != TOKEN_SYMBOL &&
				(tk + 1)->type != TOKEN_SYMBOL);
			continue;
		}
		prod_append(p, tk->value);
	}
	nonterminal_addprod(X, p);
	return tk - start;
}

static enum tktype
divisor(size_t index)
{
	switch (index) {
	case 0:
		return TOKEN_ARROW;
	default:
		return TOKEN_BAR;
	}
}

static size_t
grammar_parsenonterm(Grammar *G, Token *start)
{
	Token *tk = start;
	if (tk->type != TOKEN_SYMBOL) {
		fprintf(stderr, "nonterminal begins with %s\n",
			tktype_str(tk->type));
		exit(EXIT_FAILURE);
	}
	Nonterminal *X = map_getorset(G->map, tk->value);
	assert(X != NULL);
	tk++;
	if (tk->type != TOKEN_ARROW) {
		fprintf(stderr, "%s in place of %s\n", tktype_str(tk->type),
			tktype_str(TOKEN_ARROW));
		exit(EXIT_FAILURE);
	}
	/* nonterminal_parseprod advances X->n */
	while (tk->type == divisor(X->n)) {
		size_t len = nonterminal_parseprod(X, ++tk);
		tk += len;
	}
	if (tk->type & (TOKEN_SEMICOLON | TOKEN_EOF)) {
		return tk - start;
	}
	fprintf(stderr, "nonterminal ends with %s\n", tktype_str(tk->type));
	exit(EXIT_FAILURE);
}

static void
grammar_parsesymbols(Grammar *G, Token *tk)
{
	switch (tk->type) {
	case TOKEN_EOF:
		return;
	case TOKEN_SEMICOLON:
		grammar_parsesymbols(G, ++tk);
		return;
	default:
		/* parse nonterminal and recurses after shifting forward */
		grammar_parsesymbols(G, tk + grammar_parsenonterm(G, tk));
		return;
	}
}

static Grammar *
grammar_fromtokens(Token *tk)
{
	/* start symbol */
	if (tk->type != TOKEN_SYMBOL) {
		fprintf(stderr, "grammar begins with %s\n",
			tktype_str(tk->type));
		exit(EXIT_FAILURE);
	}
	Grammar *G = grammar_create(tk->value);
	grammar_parsesymbols(G, tk);
	token_cleanup(tk);
	return G;
}

Grammar *
grammar_parse(char *input)
{
	return grammar_fromtokens(lex(input));
}

#endif
