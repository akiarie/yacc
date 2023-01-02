#ifndef BNF_GRAMMAR
#define BNF_GRAMMAR
#include <stdlib.h>
#include <stdbool.h>

#define SYMBOL_EPSILON "<e>"
#define SYMBOL_EOF "$"
#define SYMBOL_DOT "."
#define SYMBOL_ARROW "->"

typedef struct grammar Grammar;

typedef struct symbolset {
	size_t n;
	char **sym;
	char *action;
} Prod;

Prod *
prod_create(char *action);

void
prod_destroy(Prod *p);

Prod *
prod_inline_act(char *action, ...);

#define prod_inline(action, ...) prod_inline_act(action, __VA_ARGS__, NULL)

char *
prod_bare_str(const Prod *);

char *
prod_str(const Prod *, const Grammar *G);

int
prod_append(Prod *, char *);

bool
prod_eq(Prod *, Prod *);

bool
prod_isepsilon(Prod *);

typedef struct prodset {
	size_t n;
	Prod **prod;
} Symbol;

Symbol *
symbol_create();

void
symbol_destroy(Symbol *);

void
symbol_addprod(Symbol *, Prod *);

Symbol *
symbol_inline_act(Prod *, ...);

#define symbol_inline(...) symbol_inline_act(__VA_ARGS__, NULL)

char *
symbol_str(const Symbol *, const Grammar *);

struct grammar {
	char *S; /* start symbol */
	struct map *map;
};

Grammar *
grammar_create(char *S);

/*
 * grammar_parse: we parse using the following grammar:
 * 	grammar	→ nonterm
 * 		| nonterm ';' grammar
 * 	nonterm	→ symbol ':' prods
 * 	prods	→ prod
 * 		| prod '|' prods
 * 	prod	→ symbol
 * 		| symbol prod		// symbol unspecified
 */
Grammar *
grammar_parse(char *input);

char *
grammar_str(const Grammar *);

/* gprintf: same as printf but accepts '%j' to represent a grammar and uses
 * grammar_str to print it */
int
gprintf(char *fmt, ...);

void
grammar_destroy(Grammar *);

bool
grammar_eq(Grammar *, Grammar *);

Grammar *
grammar_unleftrec(const Grammar *);

Grammar *
grammar_leftfactor(const Grammar *);

typedef struct symbolset Symbolset;

Symbolset *
symbolset_create_act(char *, ...);

#define symbolset_create(...) symbolset_create_act(__VA_ARGS__, NULL)

char *
symbolset_str(const Symbolset *);

bool
symbolset_eq(Symbolset *, Symbolset *);

int
symbolset_getindex(Symbolset *, char *sym);

int
symbolset_include(Symbolset *, char *sym);

Symbolset *
grammar_first(const Grammar *, char *sym);

Symbolset *
grammar_follow(const Grammar *, char *sym);

Symbolset *
grammar_symbolsetfirst(const Grammar *, Symbolset *);

bool
grammar_isLL1(const Grammar *);

Grammar *
grammar_augment(const Grammar *);

typedef struct {
	char *sym;
	Prod *p;
	int dot;
} Item;

Item
item_create(char *, Prod *, int);

char *
item_nextsym(Item);

bool
item_eq(Item, Item);

char *
item_str(Item, Grammar *);

typedef struct {
	Item *item;
	size_t n;
} Itemset;

Itemset
itemset_create();

void
itemset_add(Itemset *I, Item item);

Itemset
itemset_closure(Itemset, Grammar *);

Itemset
itemset_goto(Itemset, Grammar *, char *sym);

bool
itemset_eq(Itemset, Itemset);

void
itemset_destroy(Itemset);

char *
itemset_str(Itemset, Grammar *);

#endif
