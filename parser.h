#ifndef YACC_PARSER
#define YACC_PARSER

enum actiontype {
	ACTION_ACCEPT	= 1 << 0,
	ACTION_SHIFT	= 1 << 1,
	ACTION_REDUCE	= 1 << 2,
};

typedef struct {
	enum actiontype type;
	union actunion {
		int prod;	/* reduce */
		int state;	/* shift */
	} u;
} Action;

Action *
action_accept();

Action *
action_shift(int st);

Action *
action_reduce(int prod);

#define DEFAULT_TERM_VALUE 257

typedef struct {
	Itemset *state;
	struct map **action;
	size_t nstate;
	char *S;	/* start symbol, used for acceptance */
	struct lrprodset {
		char **sym;
		Prod **prod;
		size_t n;
	} prods;
	struct map *yyterms; /* map Grammar terminals to yylex int values */
	char *precode, *postcode;
} Parser;

Parser
parser_create(Grammar *, char *precode, char *postcode);

Parser
parser_create_term(Grammar *, char *, char *, struct map *yyterms);

void
parser_destroy(Parser);

char *
parser_str(Parser);

char *
parser_str_ordered(Parser, Symbolset *order);

#endif
