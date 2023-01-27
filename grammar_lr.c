#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "grammar.h"
#include "table.h"
#include "util.h"

#ifndef BNF_GRAMMAR_LR
#define BNF_GRAMMAR_LR

char *
grammar_prime(const Grammar *G, const char *X);

char *
prod_head(Prod *p);

Nonterminal *
nonterminal_copy(const Nonterminal *X);

Prod *
prod_subrange(const Prod *p, unsigned int start, unsigned int end);

Grammar *
grammar_augment(const Grammar *G)
{
	Grammar *GG = grammar_create(grammar_prime(G, G->S));
	Nonterminal *SS = nonterminal_create();
	nonterminal_addprod(SS, prod_inline("", G->S));
	map_set(GG->map, GG->S, SS);
	for (int i = 0; i < G->map->n; i++) {
		struct entry e = G->map->entry[i];
		map_set(GG->map, e.key, nonterminal_copy(e.value));
	}
	return GG;
}

Itemset
itemset_create()
{
	return (Itemset) {.item = NULL, .n = 0};
}

static Itemset
itemset_copy(Itemset I)
{
	Itemset J = (Itemset) { calloc(I.n, sizeof(Item)), I.n };
	for (int i = 0; i < I.n; i++) {
		J.item[i] = I.item[i];
	}
	return J;
}

static bool
item_empty(Item);

static bool
hasepsilon(Nonterminal *X)
{
	for (int i = 0; i < X->n; i++) {
		if (prod_isepsilon(X->prod[i])) {
			return true;
		}
	}
	return false;
}

void
itemset_add(Itemset *I, Item item)
{
	if (item_empty(item)) {
		return;
	}
	I->item = realloc(I->item, sizeof(Item) * ++I->n);
	I->item[I->n - 1] = item;
}

void
itemset_addrange(Itemset *I, Itemset J)
{
	for (int i = 0; i < J.n; i++) {
		itemset_add(I, J.item[i]);
	}
}

static Item
item_goto(Item item, char *sym);

Itemset
itemset_goto(Itemset I, Grammar *G, char *sym)
{
	assert(strcmp(sym, SYMBOL_EPSILON) != 0);
	Itemset J = itemset_create();
	for (int i = 0; i < I.n; i++) {
		itemset_add(&J, item_goto(I.item[i], sym));
	}
	return J;
}

static Itemset
itemset_fromsym(char *sym, Grammar *G)
{
	Itemset I = itemset_create();
	Nonterminal *X = map_get(G->map, sym);
	if (!X) {
		return I;
	}
	for (int i = 0; i < X->n; i++) {
		itemset_add(&I, item_create(sym, X->prod[i], 0));
	}
	return I;
}

Itemset
itemset_closure_act(Itemset I, Grammar *G, struct map *map)
{
	Itemset J = itemset_create();
	itemset_addrange(&J, I);
	for (int i = 0; i < I.n; i++) {
		char *nextsym = item_nextsym(I.item[i]);
		if (map_get(map, nextsym)) {
			continue;
		}
		map_set(map, nextsym, (void *) true);
		Itemset K = itemset_fromsym(nextsym, G);
		itemset_addrange(&J, itemset_closure_act(K, G, map));
	}
	return J;
}

Itemset
itemset_closure(Itemset I, Grammar *G)
{
	struct map *map = map_create();
	Itemset J = itemset_closure_act(I, G, map);
	map_destroy(map);
	return J;
}

bool
itemset_eq(Itemset I, Itemset J)
{
	if (I.n != J.n) {
		return false;
	}
	for (int i = 0; i < I.n; i++) {
		if (!item_eq(I.item[i], J.item[i])) {
			return false;
		}
	}
	return true;
}

void
itemset_destroy(Itemset I)
{
	free(I.item);
}

char *
itemset_str(Itemset I, Grammar *G)
{
	Table T = table_create(1);
	for (int i = 0; i < I.n; i++) {
		struct strbuilder *b = strbuilder_create();
		strbuilder_printf(b, " %s ", item_str(I.item[i], G));
		table_append(&T, row_inline("s", strbuilder_build(b)));
	}
	char *output = table_str(&T, PM_EXTERIOR, "l");
	table_destroy(&T);
	return output;
}

Item
item_create(char *sym, Prod *p, int dot)
{
	assert(0 <= dot && dot <= p->n);
	return (Item) {sym, p, dot};
}

char *
item_str(Item item, Grammar *G)
{
	assert(item.p->n > 0);
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "%s %s ", item.sym, SYMBOL_ARROW);
	Prod *left = prod_subrange(item.p, 0, item.dot),
	     *right = prod_subrange(item.p, item.dot, item.p->n);
	if (left->n > 0) {
		strbuilder_printf(b, "%s", symbolset_str(left));
	}
	strbuilder_printf(b, SYMBOL_DOT);
	if (right->n > 0) {
		strbuilder_printf(b, "%s", symbolset_str(right));
	}
	return strbuilder_build(b);
}

char *
item_nextsym(Item item)
{
	return prod_head(prod_subrange(item.p, item.dot, item.p->n));
}

bool
item_eq(Item a, Item b)
{
	return strcmp(a.sym, b.sym) == 0
		&& ( (!a.p && !b.p) || prod_eq(a.p, b.p) /* both NULL or equal */)
		&& a.dot == b.dot;
}

static Item ITEM_EMPTY = (Item) {SYMBOL_EPSILON, NULL, -1};

static bool
item_empty(Item item)
{
	return item_eq(item, ITEM_EMPTY);
}

static Item
item_goto(Item item, char *sym)
{
	assert(strcmp(sym, SYMBOL_EPSILON) != 0);
	char *nextsym = item_nextsym(item);
	if (strcmp(sym, nextsym) == 0) {
		return item_create(item.sym, item.p, item.dot + 1);
	} else {
		return ITEM_EMPTY;
	}
}

#endif
