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

#include "grammar_lr.c"
#include "grammar_parse.c"
#include "grammar_util.c"

bool
isliteral(char *sym)
{
	size_t len = strlen(sym);
	/* FIXME: upgrade this to check string literal according to C's rules */
	return len == 1 || ( len == 2 && sym[0] == '\\' && 
		(strchr("tnvfr\\", sym[1]) != NULL) );
}

Prod *
prod_create(char *action)
{
	Prod *p = (Prod *) calloc(1, sizeof(Prod));
	p->action = action;
	return p;
}

Prod *
prod_inline_act(char *action, ...)
{
	Prod *p = prod_create(action);
	va_list ap;
	va_start(ap, action);
	char *sym;
	while ((sym = va_arg(ap, char *)) != NULL) {
		prod_append(p, sym);
	}
	va_end(ap);
	return p;
}

void
prod_destroy(Prod *p)
{
	for (int i = 0; i < p->n; i++) {
		free(p->sym[i]);
	}
	free(p);
}

bool
prod_eq(Prod *p, Prod *q)
{
	assert(p && q);
	if (p->n != q->n) {
		return false;
	}
	for (int i = 0; i < p->n; i++) {
		if (strcmp(p->sym[i], q->sym[i]) != 0) {
			return false;
		}
	}
	return true;
}

static Prod *
prod_epsilon()
{
	return prod_create(NULL);
}

bool
prod_isepsilon(Prod *p)
{
	return p->n == 0;
}


static bool
prod_startswith(Prod *p, const char *sym)
{
	return !prod_isepsilon(p) && strcmp(p->sym[0], sym) == 0;
}

int
prod_append(Prod *p, char *sym)
{
	p->sym = (char **) realloc(p->sym, sizeof(char *) * ++p->n);
	p->sym[p->n - 1] = dynamic_str(sym);
	return p->n - 1;
}

void
prod_appendrange(Prod *p, Prod *q)
{
	for (int i = 0; i < q->n; i++) {
		prod_append(p, q->sym[i]);
	}
}

Prod *
prod_subrange(const Prod *p, unsigned int start, unsigned int end)
{
	assert(start <= p->n && end <= p->n);
	Prod *q = prod_create(NULL);
	for (int i = start; i < end; i++) {
		prod_append(q, p->sym[i]);
	}
	return q;
}

static Prod *
prod_copy(const Prod *p)
{
	Prod *q = prod_subrange(p, 0, p->n);
	q->action = p->action;
	return q;
}

static Prod *
prod_tail(const Prod *p)
{
	assert(p->n > 0);
	return prod_subrange(p, 1, p->n);
}

static char *
grammar_symbol_name(const Grammar *, char *);

char *
prod_bare_str(const Prod *p)
{
	if (p->n == 0) {
		return dynamic_str(SYMBOL_EPSILON);
	}
	struct strbuilder *b = strbuilder_create();
	for (int i = 0; i < p->n; i++) {
		strbuilder_printf(b, "%s%s",
			p->sym[i],
			(i + 1 < p->n ? " " : "" /* spacing */)
		);
	}
	return strbuilder_build(b);
}

char *
prod_str(const Prod *p, const Grammar *G)
{
	if (p->n == 0) {
		return dynamic_str(SYMBOL_EPSILON);
	}
	struct strbuilder *b = strbuilder_create();
	for (int i = 0; i < p->n; i++) {
		strbuilder_printf(b, "%s%s",
			grammar_symbol_name(G, p->sym[i]),
			(i + 1 < p->n) ? " " : "" /* spacing */
		);
	}
	return strbuilder_build(b);
}

Nonterminal *
nonterminal_create()
{
	return (Nonterminal *) calloc(1, sizeof(Nonterminal));
}

Nonterminal *
nonterminal_copy(const Nonterminal *X)
{
	Nonterminal *Y = nonterminal_create();
	for (int i = 0; i < X->n; i++) {
		nonterminal_addprod(Y, prod_copy(X->prod[i]));
	}
	return Y;
}

Nonterminal *
nonterminal_inline_act(Prod *p0, ...)
{
	Nonterminal *X = nonterminal_create();
	va_list ap;
	va_start(ap, p0);
	for (Prod *p = p0; p; p = va_arg(ap, Prod *)) {
		nonterminal_addprod(X, p);
	}
	va_end(ap);
	return X;
}

void
nonterminal_destroy(Nonterminal *X)
{
	for (int i = 0; i < X->n; i++) {
		prod_destroy(X->prod[i]);
	}
	free(X);
}

void
nonterminal_addprodind(Nonterminal *X, Prod *p, int index)
{
	assert(X != NULL && index <= X->n);
	/* ensure epsilon remains on end */
	if (index > 0 && prod_isepsilon(X->prod[index - 1])) {
		index -= 1;
	}
	X->prod = (Prod **) realloc(X->prod, sizeof(Prod *) * ++X->n);
	for (int i = X->n - 1; i > index; i--) {
		X->prod[i] = X->prod[i - 1];
	}
	X->prod[index] = p;
}

void
nonterminal_addprod(Nonterminal *X, Prod *p)
{
	nonterminal_addprodind(X, p, X->n);
}

static int
nonterminal_mustgetprodindex(Nonterminal *X, Prod *p)
{
	for (int i = 0; i < X->n; i++) {
		if (prod_eq(p, X->prod[i])) {
			return i;
		}
	}
	assert(false);
}

/* nonterminal_delprod: remove production and return its index */
static int
nonterminal_delprod(Nonterminal *X, Prod *p)
{
	int index = nonterminal_mustgetprodindex(X, p);
	for (int i = index + 1; i < X->n; i++) {
		X->prod[i - 1] = X->prod[i];
	}
	X->prod = (Prod **) realloc(X->prod, sizeof(Prod *) * --X->n);
	return index;
}

static bool
nonterminal_eq(const Nonterminal *X, const Nonterminal *Y)
{
	if (X->n != Y->n) {
		return false;
	}
	for (int i = 0; i < X->n; i++) {
		if (!prod_eq(X->prod[i], Y->prod[i])) {
			return false;
		}
	}
	return true;
}

char *
nonterminal_str(const Nonterminal *X, const Grammar *G)
{
	struct strbuilder *b = strbuilder_create();
	for (int i = 0; i < X->n; i++) {
		strbuilder_printf(b, "%s%s",
			prod_str(X->prod[i], G),
			(i + 1 < X->n) ? " | " : "" /* spacing */
		);
	}
	return strbuilder_build(b);
}

Grammar *
grammar_create(char *S)
{
	Grammar *G = malloc(sizeof(Grammar));
	G->S = dynamic_str(S);
	G->map = map_create();
	return G;
}

void
grammar_destroy(Grammar *G)
{
	for (int i = 0; i < G->map->n; i++) {
		nonterminal_destroy((Nonterminal *) G->map->entry[i].value);
	}
	map_destroy(G->map);
	free(G->S);
	free(G);
}

static int
max(int a, int b)
{
	return (a > b) ? a : b;
}

static int
maxntlen(const Grammar *G)
{
	int len = 0;
	for (int i = 0; i < G->map->n; i++) {
		struct entry e = G->map->entry[i];
		Nonterminal *X = (Nonterminal *) e.value;
		if (X->n == 0) {
			continue;
		}
		len = max(len, strlen(e.key));
	}
	return len;
}

#define COLOUR_BLUE  "\033[34m"
#define COLOUR_OFF   "\e[m"

static char *
grammar_symbol_name(const Grammar *G, char *sym)
{
	if (map_get(G->map, sym) != NULL) { /* nonterminal */
		return sym;
	}
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "%s%s%s", COLOUR_BLUE, sym, COLOUR_OFF);
	return strbuilder_build(b);
}

static char *
grammar_symbol_str(const Grammar *G, const char *sym, int padding)
{
	Nonterminal *X = map_get(G->map, sym);
	assert(X != NULL);
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "%-*s→ %s", padding, sym, nonterminal_str(X, G));
	return strbuilder_build(b);
}

char *
grammar_str(const Grammar *G)
{
	struct strbuilder *b = strbuilder_create();
	Nonterminal *S = map_get(G->map, G->S);
	assert(S != NULL);
	int padding = maxntlen(G) + 1;
	strbuilder_printf(b, "%s", grammar_symbol_str(G, G->S, padding));
	for (int i = 0; i < G->map->n; i++) {
		struct entry e = G->map->entry[i];
		Nonterminal *X = (Nonterminal *) e.value;
		if (X->n == 0 || strcmp(e.key, G->S) == 0) {
			continue;
		}
		strbuilder_printf(b, "\n%s",
			grammar_symbol_str(G, e.key, padding));
	}
	return strbuilder_build(b);
}

static bool
grammar_eq_act(Grammar *G, Grammar *H)
{
	for (int i = 0; i < G->map->n; i++) {
		struct entry e = G->map->entry[i];
		Nonterminal *X = map_get(H->map, e.key);
		if (!X || !nonterminal_eq(X, e.value)) {
			return false;
		}
	}
	return true;
}

bool
grammar_eq(Grammar *G, Grammar *H)
{
	return grammar_eq_act(G, H) && grammar_eq_act(H, G);
}

char *
grammar_prime(const Grammar *G, const char *X)
{
	char *s = dynamic_str(X);
	while (map_get(G->map, s)) {
		int len = strlen(s) + 1 + 1;
		s = realloc(s, sizeof(char) * len);
		snprintf(s, len, "%s'", s);
	}
	return s;
}

static void
grammar_overstep(Grammar *G, const char *symX, const char *symY)
{
	Nonterminal *X = map_get(G->map, symX),
	       *Y = map_get(G->map, symY);
	assert(X != NULL && Y != NULL);
	for (int i = 0; i < X->n; i++) {
		Prod *p = X->prod[i];
		if (!prod_startswith(p, symY)) {
			continue;
		}
		/* remove X → p */
		int index = nonterminal_delprod(X, p);
		/* add to X → q·tail(p) for each Y → q */
		Prod *tail = prod_tail(p);
		for (int j = 0; j < Y->n; j++) {
			Prod *q = prod_copy(Y->prod[j]);
			prod_appendrange(q, tail);
			nonterminal_addprodind(X, q, index++);
		}
	}
}

static void
grammar_unleftrec_immediate(Grammar *G, const char *sym)
{
	Nonterminal *X = map_get(G->map, sym); assert(X != NULL);
	Nonterminal *new = nonterminal_create();
	Nonterminal *X_ = nonterminal_create();
	char *prime = grammar_prime(G, sym);
	for (int i = 0; i < X->n; i++) {
		Prod *p = X->prod[i];
		if (prod_startswith(p, sym)) {
			/* add tail(p)·X_ to X_ */
			Prod *tail = prod_tail(p);
			prod_append(tail, prime);
			nonterminal_addprod(X_, tail);
		} else {
			/* add p·X_ to newX */
			Prod *q = prod_copy(p);
			prod_append(q, prime);
			nonterminal_addprod(new, q);
		}
	}
	if (X_->n == 0) { /* no left recursion */
		nonterminal_destroy(new);
		nonterminal_destroy(X_);
		goto done;
	}
	nonterminal_addprod(X_, prod_epsilon());
	map_setow(G->map, sym, new, nonterminal_destroy);
	map_set(G->map, prime, X_);
done:
	free(prime);
}

static Grammar *
grammar_copy(const Grammar *G)
{
	Grammar *H = grammar_create(G->S);
	for (int i = 0; i < G->map->n; i++) {
		struct entry e = G->map->entry[i];
		map_set(H->map, e.key, nonterminal_copy(e.value));
	}
	return H;
}

Grammar *
grammar_unleftrec(const Grammar *G)
{
	Grammar *H = grammar_copy(G);
	/* must iterate on G's map because H->map is changing */
	for (int i = 0; i < G->map->n; i++) {
		const char *sym = G->map->entry[i].key;
		for (int j = 0; j < i; j++) {
			grammar_overstep(H, sym, G->map->entry[j].key);
		}
		grammar_unleftrec_immediate(H, sym);
	}
	return H;
}

static bool
grammar_leftfactor_sym(const Grammar *, const char *);

static bool
grammar_leftfactor_sym_act(const Grammar *G, const char *sym,
		struct prefixnode *np) {
	Nonterminal *X = map_get(G->map, sym); assert(X != NULL);
	struct prefixnode *gcf = prefixnode_gcf(np);
	assert(gcf != NULL && gcf->nprod > 0);
	if (gcf->nprod == 1) { /* no common prefixes */
		return false;
	}
	Nonterminal *new = nonterminal_copy(X);
	Nonterminal *X_ = nonterminal_create();
	for (int i = 0; i < gcf->nprod; i++) {
		Prod *p = X->prod[gcf->prod[i]];
		/* remove new → p */
		nonterminal_delprod(new, p);
		/* add X_ → tail(p) for p in gcf */
		Prod *q = prefixnode_prodtail(gcf, p);
		nonterminal_addprod(X_, q);
	}
	/* add new → pref·X_ at the index of the first */
	char *prime = grammar_prime(G, sym);
	Prod *pref = prefixnode_commonprefix(gcf, X);
	prod_append(pref, prime);
	nonterminal_addprodind(new, pref, gcf->prod[0]);
	map_setow(G->map, sym, new, nonterminal_destroy);
	map_set(G->map, prime, X_);
	while (grammar_leftfactor_sym(G, sym)) {}
	grammar_leftfactor_sym(G, prime);
	free(prime);
	return true;
}

static bool
grammar_leftfactor_sym(const Grammar *G, const char *sym)
{
	struct prefixnode *np = prefixnode_fromsymbol(G, sym);
	bool factored = grammar_leftfactor_sym_act(G, sym, np);
	prefixnode_destroy(np);
	return factored;
}

/* grammar_leftfactor: left factor a grammar. the book is slightly ambiguous in
 * speaking of "the longest prefix α common to two or more [of each
 * nonterminal's] alternatives": this phrase doesn't specify whether the length
 * or the commonality should dominate. this implementation allows commonality to
 * dominate to avoid repeated applications. */
Grammar *
grammar_leftfactor(const Grammar *G)
{
	Grammar *H = grammar_copy(G);
	/* must iterate on G's map because H->map is changing */
	for (int i = 0; i < G->map->n; i++) {
		grammar_leftfactor_sym(H, G->map->entry[i].key);
	}
	return H;
}

static void
symbolset_addfirst(const Grammar *G, Symbolset *set, Prod *p,
		struct circuitbreaker *tr)
{
	if (p->n == 0 || !circuitbreaker_append(tr, p->sym[0])) {
		return;
	}
	Prod *pfirst = grammar_first(G, p->sym[0]);
	prod_appendrange(set, pfirst);
	for (int i = 0; i < pfirst->n; i++) {
		if (strcmp(pfirst->sym[i], SYMBOL_EPSILON) == 0){
			symbolset_addfirst(G, set, prod_subrange(p, 1, p->n), tr);
			return;
		}
	}
}

int
symbolset_getindex(Symbolset *set, char *sym)
{
	for (int i = 0; i < set->n; i++) {
		if (strcmp(set->sym[i], sym) == 0) {
			return i;
		}
	}
	return -1;
}

bool
symbolset_eq(Symbolset *s1, Symbolset *s2)
{
	for (int i = 0; i < s1->n; i++) {
		if (symbolset_getindex(s2, s1->sym[i]) < 0) {
			return false;
		}
	}
	return s1->n == s2->n;
}

int
symbolset_include(Symbolset *set, char *sym)
{
	int index = symbolset_getindex(set, sym);
	return index < 0 ? prod_append(set, sym) : index;
}

static void
symbolset_includerange(Symbolset *p, Symbolset *q)
{
	for (int i = 0; i < q->n; i++) {
		symbolset_include(p, q->sym[i]);
	}
}

Symbolset *
symbolset_create_act(char *s, ...)
{
	Symbolset *set = prod_create(NULL);
	va_list ap;
	va_start(ap, s);
	for (char *sym = s; sym; sym = va_arg(ap, char *)) {
		symbolset_include(set, sym);
	}
	va_end(ap);
	return set;
}

Symbolset *
grammar_first(const Grammar *G, char *sym)
{
	Symbolset *set = prod_create(NULL);
	Nonterminal *X = map_get(G->map, sym);
	if (X == NULL) { /* X terminal */
		symbolset_include(set, sym);
		return set;
	}
	/* X nonterminal */
	for (int i = 0; i < X->n; i++) {
		Prod *p = X->prod[i];
		if (prod_isepsilon(p)) {
			symbolset_include(set, SYMBOL_EPSILON);
			continue;
		}
		struct circuitbreaker *tr = circuitbreaker_create(sym);
		symbolset_addfirst(G, set, p, tr);
		circuitbreaker_destroy(tr);
	}
	return set;
}

static Symbolset *
symbolset_without(Symbolset *set, char *sym)
{
	Symbolset *setwo = prod_create(NULL);
	for (int i = 0; i < set->n; i++) {
		if (strcmp(set->sym[i], sym) != 0) {
			prod_append(setwo, set->sym[i]);
		}
	}
	return setwo;
}

Symbolset *
grammar_symbolsetfirst(const Grammar *G, Symbolset *set)
{
	Symbolset *first = prod_create(NULL);
	for (int i = 0; i < set->n; i++) {
		Symbolset *next = grammar_first(G, set->sym[i]);
		symbolset_includerange(first, symbolset_without(next,
			SYMBOL_EPSILON));
		/* abort if current symbol's FIRST doesn't have ε */
		if (symbolset_getindex(next, SYMBOL_EPSILON) < 0) {
			return first;
		}
	}
	symbolset_include(first, SYMBOL_EPSILON);
	return first;
}

char *
prod_head(Prod *p)
{
	return p->n > 0 ? p->sym[0] : SYMBOL_EPSILON;
}

static Symbolset *
symbolset_firstafter(const Grammar *G, Symbolset *set, char *sym)
{
	Symbolset *after = prod_create(NULL);
	int k = 0;
	while ((k = symbolset_getindex(set, sym)) >= 0) {
		set = prod_subrange(set, k + 1, set->n);
		symbolset_includerange(after, grammar_first(G, prod_head(set)));
	}
	return after;
}

static bool
symbolset_followsubset(const Grammar *G, char *sym, Nonterminal *X)
{
	for (int i = 0; i < X->n; i++) {
		Symbolset *set = symbolset_firstafter(G, X->prod[i], sym);
		if (symbolset_getindex(set, SYMBOL_EPSILON) != -1) {
			return true;
		}
	}
	return false;
}

char *
symbolset_str(const Symbolset *set)
{
	if (set->n == 0) {
		return SYMBOL_EPSILON;
	}
	struct strbuilder *b = strbuilder_create();
	for (int i = 0; i < set->n; i++) {
		strbuilder_printf(b, "%s%s", set->sym[i],
				(i + 1 < set->n ? " " : "") /* spacing */);
	}
	return strbuilder_build(b);
}

static Symbolset *
grammar_follow_act(const Grammar *G, char *sym, struct circuitbreaker *tr);

static Symbolset *
symbolset_prodfollow(const Grammar *G, char *sym, Prod *p)
{
	Symbolset *set = prod_create(NULL);
	Symbolset *fstafter = symbolset_firstafter(G, p, sym);
	symbolset_includerange(set, symbolset_without(fstafter, SYMBOL_EPSILON));
	return set;
}

static Symbolset *
symbolset_computefollow(const Grammar *G, char *sym, char *symX,
		struct circuitbreaker *tr)
{
	Symbolset *set = prod_create(NULL);
	Nonterminal *X = map_get(G->map, symX);
	assert(X != NULL);
	/* 2. */
	for (int i = 0; i < X->n; i++) {
		symbolset_includerange(set,
			symbolset_prodfollow(G, sym, X->prod[i]));
	}
	/* 3. */
	if (symbolset_followsubset(G, sym, X)
			&& circuitbreaker_append(tr, symX)) {
		symbolset_includerange(set, grammar_follow_act(G, symX, tr));
	}
	return set;
}

static Symbolset *
grammar_follow_act(const Grammar *G, char *sym, struct circuitbreaker *tr)
{
	Symbolset *set = prod_create(NULL);
	/* 1. */
	if (strcmp(G->S, sym) == 0) {
		symbolset_include(set, SYMBOL_EOF);
	}
	/* 2. and 3. */
	for (int i = 0; i < G->map->n; i++) {
		struct entry e = G->map->entry[i];
		Symbolset *fllws = symbolset_computefollow(G, sym, e.key, tr);
		symbolset_includerange(set, fllws);
	}
	return set;
}

Symbolset *
grammar_follow(const Grammar *G, char *sym)
{
	struct circuitbreaker *tr = circuitbreaker_create(sym);
	Symbolset *set = grammar_follow_act(G, sym, tr);
	circuitbreaker_destroy(tr);
	return set;
}

/* grammar_sym_LL1cond12: find and return a violation of conditions (1) and
 * (2) of the definition of an LL(1) grammar, or return NULL otherwise. */
static char *
grammar_sym_LL1cond12(const Grammar *G, char *sym)
{
	Nonterminal *X = map_get(G->map, sym);
	assert(X != NULL);
	Symbolset *enc = prod_create(NULL); /* symbols already encountered */
	for (int i = 0; i < X->n; i++) {
		Prod *p = X->prod[i];
		Symbolset *first = grammar_symbolsetfirst(G, (Symbolset *) p);
		for (int j = 0; j < first->n; j++) {
			char *s = first->sym[j];
			if (symbolset_getindex(enc, s) != -1) {
				return s;
			}
			/* prod_append since we know it's not in the set */
			prod_append(enc, s);
		}
	}
	return NULL;
}

/* grammar_sym_prodeps: return index of first production with ε in its FIRST, or
 * -1 if no such production exists. */
static int
grammar_sym_prodeps(const Grammar *G, Nonterminal *X)
{
	for (int i = 0; i < X->n; i++) {
		Prod *p = X->prod[i];
		Symbolset *first = grammar_symbolsetfirst(G, (Symbolset *) p);
		if (symbolset_getindex(first, SYMBOL_EPSILON) != -1) {
			return i;
		}
	}
	return -1;
}

static bool
symbolset_disjoint(Symbolset *s1, Symbolset *s2)
{
	for (int i = 0; i < s1->n; i++) {
		if (symbolset_getindex(s2, s1->sym[i]) != -1) {
			return false;
		}
	}
	return true;
}

/* grammar_sym_LL1cond3: same as grammar_sym_LL1cond12 but for condition (3). */
static char *
grammar_sym_LL1cond3(const Grammar *G, char *sym)
{
	Nonterminal *X = map_get(G->map, sym);
	assert(X != NULL);
	int eps = grammar_sym_prodeps(G, X);
	if (eps < 0) {
		return NULL;
	}
	Symbolset *follw = grammar_follow(G, sym);
	for (int i = 0; i < X->n; i++) {
		if (i == eps) {
			continue;
		}
		Prod *p = X->prod[i];
		Symbolset *first = grammar_symbolsetfirst(G, (Symbolset *) p);
		if (!symbolset_disjoint(first, follw)) {
			return prod_str(p, G);
		}
	}
	return NULL;
}

static bool
grammar_symLL1(const Grammar *G, char *sym)
{
	return !grammar_sym_LL1cond12(G, sym) && !grammar_sym_LL1cond3(G, sym);
}

bool
grammar_isLL1(const Grammar *G)
{
	for (int i = 0; i < G->map->n; i++) {
		if (!grammar_symLL1(G, G->map->entry[i].key)) {
			return false;
		}
	}
	return true;
}
