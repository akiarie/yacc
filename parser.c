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
	P->action = realloc(P->action, sizeof(struct map *) * P->nstate);
	P->state[i] = state;
	P->action[i] = map_create();
	return i;
}

static Action *
action_create(enum actiontype type)
{
	Action *act = malloc(sizeof(Action));
	act->type = type;
	return act;
}

Action *
action_accept()
{
	return action_create(ACTION_ACCEPT);
}

Action *
action_shift(int st)
{
	Action *act = action_create(ACTION_SHIFT);
	act->u = (union actunion) { .state = st };
	return act;
}

Action *
action_reduce(int prod)
{
	Action *act = action_create(ACTION_REDUCE);
	act->u = (union actunion) { .prod = prod };
	return act;
}

static void
action_destroy(Action *act)
{
	free(act);
}

static Action *
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
	struct map *action = P->action[st];
	for (int i = 0; i < I.n; i++) {
		char *nextsym = item_nextsym(I.item[i]);
		if (map_get(action, nextsym) != NULL) {
			continue;
		}
		if (strcmp(nextsym, SYMBOL_EPSILON) == 0) {
			Symbolset *fllw = grammar_follow(G, I.item[i].sym);
			for (int j = 0; j < fllw->n; j++) {
				Action *act = reduceacc(P, I.item[i]);
				map_set(action, fllw->sym[j], act);
			}
			continue;
		}
		Itemset next = itemset_goto(I, G, nextsym);
		int index = parser_includestate(P, itemset_closure(next, G));
		map_set(action, nextsym, action_shift(index));
	}
}

static struct lrprodset
grammar_prods(Grammar *G)
{
	size_t n = 0; char **sym = NULL; Prod **prod = NULL;
	for (int i = 0; i < G->map->n; i++) {
		struct entry e = G->map->entry[i];
		Nonterminal *X = (Nonterminal *) e.value;
		for (int j = 0; j < X->n; j++) {
			int index = n++;
			sym = realloc(sym, sizeof(char *) * n);
			prod = realloc(prod, sizeof(Prod *) * n);
			sym[index] = e.key;
			prod[index] = X->prod[j];
		}
	}
	return (struct lrprodset) { sym, prod, n };
}

static Symbolset *
getterminals(Grammar *G)
{
	Symbolset *set = prod_create(NULL);
	for (int i = 0; i < G->map->n; i++) {
		Nonterminal *X = (Nonterminal *) G->map->entry[i].value;
		for (int j = 0; j < X->n; j++) {
			Prod *p = X->prod[j];
			for (int k = 0; k < p->n; k++) {
				if (!map_get(G->map, p->sym[k])) {
					symbolset_include(set, p->sym[k]);
				}
			}
		}
	}
	return set;
}

static struct map *
canonicalterms(Grammar *G, struct map *defined)
{
	struct map *map = map_create();
	unsigned long defindex = DEFAULT_TERM_VALUE;
	for (int i = 0; i < defined->n; i++) {
		struct entry e = defined->entry[i];
		unsigned long value = (unsigned long) e.value;
		if (value >= defindex) {
			defindex = value + 1;
		}
		map_set(map, e.key, e.value);
	}
	Symbolset *terminals = getterminals(G);
	for (int i = 0; i < terminals->n; i++) {
		/* only define the undefined */
		if (map_getindex(map, terminals->sym[i]) == -1) {
			map_set(map, terminals->sym[i], (void *) defindex++);
		}
	}
	prod_destroy(terminals);
	return map;
}

Parser
parser_create_term(Grammar *G, char *precode, char *postcode, struct map *terminals)
{
	Nonterminal *S = map_get(G->map, G->S); assert(S != NULL && S->n > 0);
	Itemset start = itemset_create();
	itemset_add(&start, item_create(G->S, S->prod[0], 0));
	Parser P = (Parser) {
		.S = G->S, .prods = grammar_prods(G),
		.nstate = 0, .state = NULL, .action = NULL,
		.yyterms = canonicalterms(G, terminals),
		.precode = precode, .postcode = postcode,
	};
	parser_includestate(&P, itemset_closure(start, G));
	for (int i = 0; i < P.nstate; i++) {
		stateshifts(&P, G, i);
	}
	return P;
}

Parser
parser_create(Grammar *G, char *precode, char *postcode)
{
	struct map *empty = map_create();
	Parser P = parser_create_term(G, precode, postcode, canonicalterms(G, empty));
	map_destroy(empty);
	return P;
}

void
parser_destroy(Parser P)
{
	free(P.state);
	for (int i = 0; i < P.nstate; i++) {
		struct map *action = P.action[i];
		for (int j = 0; j < action->n; j++) {
			action_destroy((Action *) action->entry[j].value);
		}
		map_destroy(action);
	}
	free(P.action);
	free(P.prods.sym);
	free(P.prods.prod);
}

static char *
safesym(char *sym)
{
	if (strcmp(sym, "\n") == 0) {
		return "\\n";
	}
	return sym;
}

static Row *
parser_title(Parser P, Symbolset *symbols)
{
	Row *title = row_create(repeat('s', symbols->n + 1));
	row_appendstring(title, "STATE");
	for (int i = 0; i < symbols->n; i++) {
		row_appendstring(title, safesym(symbols->sym[i]));
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
		strbuilder_printf(b1, "%s%s", safesym(p->sym[j]),
			(j + 1 < p->n) ? " " : "" /* spacing */);
	}
	struct strbuilder *b2 = strbuilder_create();
	if (p->action) {
		strbuilder_puts(b2, " { ");
		strbuilder_puts(b2, p->action);
		strbuilder_puts(b2, " }");
	}
	return row_inline("sss", strbuilder_build(b), strbuilder_build(b1),
		strbuilder_build(b2));
}

static char *
prodset_str(struct lrprodset set)
{
	Table T = table_create(3);
	/* start at 1 to skip augmented symbol production */
	for (int i = 1; i < set.n; i++) {
		table_append(&T, prod_row(i, set.sym[i], set.prod[i]));
	}
	char *output = table_str(&T, PM_COMPACT, "rll");
	table_destroy(&T);
	return output;
}

static char *
getaction(Itemset state, struct map *action, char *sym)
{
	struct strbuilder *b = strbuilder_create();
	Action *act = map_get(action, sym);
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
		char *act = getaction(P.state[st], P.action[st], symbols->sym[i]);
		row_appendstring(row, act);
	}
	return row;
}

char *
parser_str_ordered(Parser P, Symbolset *order)
{
	struct strbuilder *b = strbuilder_create();
	char *prods = prodset_str(P.prods);
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
