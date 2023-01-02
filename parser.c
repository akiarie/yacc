#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "grammar.h"
#include "parser.h"
#include "table.h"
#include "util.h"

static int
parser_mustgetprod(Parser P, Prod *p)
{
	for (int i = 0; i < P.prods.n; i++) {
		if (prod_eq(P.prods.prod[i], p)) {
			return i;
		}
	}
	assert(false);
}

static int
parser_includestate(Parser *P, Itemset state)
{
	for (int i = 0; i < P->nstate; i++) {
		if (itemset_eq(state, P->state[i])) {
			return i;
		}
	}
	int i = P->nstate++; /* increment length and store final index */
	P->state = realloc(P->state, sizeof(Itemset) * P->nstate);
	P->_goto = realloc(P->_goto, sizeof(struct map *) * P->nstate);
	P->state[i] = state;
	P->_goto[i] = map_create();
	return i;
}

enum actiontype {
	ACTION_ACCEPT	= 1 << 0,
	ACTION_SHIFT	= 1 << 1,
	ACTION_REDUCE	= 1 << 2,
};

struct action {
	enum actiontype type;
	union actunion {
		int prod; /* reduce */
		int state; /* shift */
	} u;
};

static struct action *
action_create(enum actiontype type)
{
	struct action *act = malloc(sizeof(struct action));
	act->type = type;
	return act;
}

static struct action *
action_accept()
{
	return action_create(ACTION_ACCEPT);
}

static struct action *
action_shift(int st)
{
	struct action *act = action_create(ACTION_SHIFT);
	act->u = (union actunion) { .state = st };
	return act;
}

static struct action *
action_reduce(int prod)
{
	struct action *act = action_create(ACTION_REDUCE);
	act->u = (union actunion) { .prod = prod };
	return act;
}

static void
action_destroy(struct action *act)
{
	free(act);
}

static struct action *
reduceacc(Parser *P, Item item)
{
	if (strcmp(item.sym, P->S) == 0) {
		return action_accept();
	} 
	return action_reduce(parser_mustgetprod(*P, item.p));
}

static void
stateshifts(Parser *P, Grammar *G, int st)
{
	Itemset I = P->state[st];
	struct map *_goto = P->_goto[st];
	for (int i = 0; i < I.n; i++) {
		char *nextsym = item_nextsym(I.item[i]);
		if (map_get(_goto, nextsym) != NULL) {
			continue;
		}
		if (strcmp(nextsym, SYMBOL_EPSILON) == 0) {
			Symbolset *fllw = grammar_follow(G, I.item[i].sym);
			for (int j = 0; j < fllw->n; j++) {
				struct action *act = reduceacc(P, I.item[i]);
				map_set(_goto, fllw->sym[j], act);
			}
			continue;
		}
		Itemset next = itemset_goto(I, G, nextsym);
		int index = parser_includestate(P, itemset_closure(next, G));
		map_set(_goto, nextsym, action_shift(index));
	}
}

static struct lrprodset
grammar_prods(Grammar *G)
{
	size_t n = 0; char **sym = NULL; Prod **prod = NULL;
	for (int i = 0; i < G->map->n; i++) {
		struct entry *e = G->map->entries[i];
		Symbol *X = (Symbol *) e->value;
		for (int j = 0; j < X->n; j++) {
			int index = n++;
			sym = realloc(sym, sizeof(char *) * n);
			prod = realloc(prod, sizeof(Prod *) * n);
			sym[index] = e->key;
			prod[index] = X->prod[j];
		}
	}
	return (struct lrprodset) { sym, prod, n };
}

Parser
parser_create(Grammar *G)
{
	Symbol *S = map_get(G->map, G->S); assert(S != NULL && S->n > 0);
	Itemset start = itemset_create();
	itemset_add(&start, item_create(G->S, S->prod[0], 0));
	Parser P = (Parser) {
		.S = G->S, .prods = grammar_prods(G),
		.nstate = 0, .state = NULL, ._goto = NULL,
	};
	parser_includestate(&P, itemset_closure(start, G));
	for (int i = 0; i < P.nstate; i++) {
		stateshifts(&P, G, i);
	}
	return P;
}

void
parser_destroy(Parser P)
{
	free(P.state);
	for (int i = 0; i < P.nstate; i++) {
		struct map *_goto = P._goto[i];
		for (int j = 0; j < _goto->n; j++) {
			action_destroy((struct action *) _goto->entries[j]->value);
		}
		map_destroy(_goto);
	}
	free(P._goto);
	free(P.prods.sym);
	free(P.prods.prod);
}

static Row *
parser_title(Parser P, Symbolset *symbols)
{
	Row *title = row_create(repeat('s', symbols->n + 1));
	row_appendstring(title, "STATE");
	for (int i = 0; i < symbols->n; i++) {
		row_appendstring(title, symbols->sym[i]);
	}
	return title;
}

static char *
inttostr(int i)
{
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "%d", i);
	return strbuilder_build(b);
}

static Row *
prod_row(int i, char *sym, Prod *p)
{
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "(%d) ", i);
	struct strbuilder *b1 = strbuilder_create();
	strbuilder_printf(b1, "%s -> ", sym);
	for (int j = 0; j < p->n; j++) {
		strbuilder_printf(b1, "%s%s", p->sym[j],
			(j + 1 < p->n) ? " " : "" /* spacing */);
	}
	return row_inline("ss", strbuilder_build(b), strbuilder_build(b1));
}

static char *
podset_str(struct lrprodset set)
{
	Table T = table_create(2);
	/* start at 1 to skip augmented symbol production */
	for (int i = 1; i < set.n; i++) {
		table_append(&T, prod_row(i, set.sym[i], set.prod[i]));
	}
	char *output = table_str(&T, PM_COMPACT, "rl");
	table_destroy(&T);
	return output;
}

static char *
getaction(Itemset state, struct map *_goto, char *sym)
{
	struct strbuilder *b = strbuilder_create();
	struct action *act = map_get(_goto, sym);
	if (act) {
		switch (act->type) {
		case ACTION_ACCEPT:
			strbuilder_printf(b, "acc");
			break;
		case ACTION_SHIFT:
			strbuilder_printf(b, "s%d", act->u.state);
			break;
		case ACTION_REDUCE:
			strbuilder_printf(b, "r%d", act->u.prod);
			break;
		}
	}
	return strbuilder_build(b);
}

static Row *
parser_staterow(Parser P, Symbolset *symbols, int st)
{
	Row *row = row_create(repeat('s', symbols->n + 1));
	row_appendstring(row, inttostr(st));
	for (int i = 0; i < symbols->n; i++) {
		char *act = getaction(P.state[st], P._goto[st], symbols->sym[i]);
		row_appendstring(row, act);
	}
	return row;
}

char *
parser_str_ordered(Parser P, Symbolset *order)
{
	struct strbuilder *b = strbuilder_create();
	char *prods = podset_str(P.prods);
	strbuilder_printf(b, "%s", prods);
	free(prods);

	size_t ncol = order->n + 1;
	Table T = table_create(ncol);
	table_append(&T, parser_title(P, order));
	for (int i = 0; i < P.nstate; i++) {
		table_append(&T, parser_staterow(P, order, i));
	}
	char *output = table_str(&T, PM_ENTIRE, repeat('c', ncol));
	strbuilder_printf(b, "%s", output);
	free(output);
	table_destroy(&T);

	return strbuilder_build(b);
}

char *
parser_str(Parser P)
{
	assert(0);
}
