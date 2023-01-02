#ifndef YACC_PARSER
#define YACC_PARSER

typedef struct {
	Itemset *state;
	struct map **_goto;
	size_t nstate;
	char *S; /* start symbol, used for acceptance */
	struct lrprodset {
		char **sym;
		Prod **prod;
		size_t n;
	} prods;
} Parser;

Parser
parser_create(Grammar *);

void
parser_destroy(Parser);

char *
parser_str(Parser);

char *
parser_str_ordered(Parser, Symbolset *order);

#endif
