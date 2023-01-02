#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "grammar.h"
#include "util.h"

#ifndef BNF_GRAMMAR_UTIL
#define BNF_GRAMMAR_UTIL

static char *
findnextfmt(char *start)
{
	char *s = start;
	for (; *s && *s != '%'; s++) {}
	int len = s - start;
	char *output = malloc(sizeof(char) * (len + 2));
	strncpy(output + 1, start, len);
	*output = '%';
	*(output + len) = '\0';
	return output;
}

int
gprintf(char *fmt, ...)
{
	va_list ap;
	char *otherfmt;
	va_start(ap, fmt);
	int tot = 0; /* total printed chars */
	for (char *p = fmt; *p; p++) {
		if (*p != '%') {
			tot++;
			putchar(*p);
			continue;
		}
		/* ⊢ *p == '%' */
		switch (*++p) {
		case 'j':
			/* grammar: print using grammar_str */
			tot += printf("%s", grammar_str(va_arg(ap, Grammar *)));
			break;
		default:
			/* other fmt verb */
			otherfmt = findnextfmt(p);
			tot += vprintf(otherfmt, ap);
			p += strlen(otherfmt);
			free(otherfmt);
			break;
		}
	}
	va_end(ap);
	return tot;
}


struct prefixnode {
	int depth;
	char *val; /* NULL for root node */
	struct prefixnode **child;
	size_t nchild;
	int *prod; /* array storing productions */
	size_t nprod;
};

static struct prefixnode *
prefixnode_create(char *val, int depth)
{
	struct prefixnode *np = calloc(1, sizeof(struct prefixnode));
	np->depth = depth;
	if (val) {
		np->val = dynamic_str(val);
	}
	return np;
}

static void
prefixnode_destroy(struct prefixnode *np)
{
	for (int i = 0; i < np->nchild; i++) {
		prefixnode_destroy(np->child[i]);
	}
	if (np->val) {
		free(np->val);
	}
	free(np);
}

static void
prefixnode_fprintf_node(struct strbuilder *b, struct prefixnode *np)
{
	strbuilder_printf(b, "[%s: {", np->val ? np->val : "<root>");
	for (int i = 0; i < np->nprod; i++) {
		strbuilder_printf(b, "%d%s", np->prod[i],
			(i + 1) < np->nprod ? ", " : "" /* comma and space */);
	}
	strbuilder_printf(b, "}, %lu, %d]\n", np->nchild, np->depth);
}

static void
findent(struct strbuilder *b, int len)
{
	for (int i = 0; i < len; i++) {
		strbuilder_printf(b, "\t");
	}
}

static char *
prefixnode_str_act(struct prefixnode *np, int level)
{
	struct strbuilder *b = strbuilder_create();
	findent(b, level);
	prefixnode_fprintf_node(b, np);
	for (int i = 0; i < np->nchild; i++) {
		struct prefixnode *npci = np->child[i];
		findent(b, level);
			strbuilder_printf(b, "|\n");
		findent(b, level);
			strbuilder_printf(b, "-- '%s' -->\n", npci->val);
		strbuilder_printf(b, "%s", prefixnode_str_act(npci, level + 1));
	}
	return strbuilder_build(b);
}

static char *
prefixnode_str(struct prefixnode *np)
{
	return prefixnode_str_act(np, 0);
}

static struct prefixnode *
prefixnode_addchild(struct prefixnode *np, char *val)
{
	np->child = (struct prefixnode **) realloc(np->child,
		sizeof(struct prefixnode *) * ++np->nchild);
	return np->child[np->nchild - 1] = prefixnode_create(val, np->depth + 1);
}

static struct prefixnode *
prefixnode_getoraddchild(struct prefixnode *np, char *val)
{
	for (int i = 0; i < np->nchild; i++) {
		if (strcmp(np->child[i]->val, val) == 0) {
			return np->child[i];
		}
	}
	return prefixnode_addchild(np, val);
}

static void
prefixnode_addprodind(struct prefixnode *np, int prodind)
{
	for (int i = 0; i < np->nprod; i++) {
		assert(np->prod[i] != prodind);
	}
	np->prod = (int *) realloc(np->prod, sizeof(int) * ++np->nprod);
	np->prod[np->nprod - 1] = prodind;
}

void
prod_destroy(Prod *p);

Prod *
prod_subrange(const Prod *p, unsigned int start, unsigned int end);

static void
prefixnode_insertprod(struct prefixnode *np, Prod *p, int prodind)
{
	prefixnode_addprodind(np, prodind);
	if (p->n == 0) {
		return;
	}
	struct prefixnode *next = prefixnode_getoraddchild(np, p->sym[0]);
	Prod *sub = prod_subrange(p, 1, p->n);
	prefixnode_insertprod(next, sub, prodind);
	prod_destroy(sub);
}

static struct prefixnode *
prefixnode_fromsymbol(const Grammar *G, const char *sym)
{
	Symbol *X = map_get(G->map, sym); assert(X != NULL);
	struct prefixnode *np = prefixnode_create(NULL, 0);
	for (int i = 0; i < X->n; i++) {
		prefixnode_insertprod(np, X->prod[i], i);
	}
	return np;
}

/* prefixnode_maxorfirst: return the node with the higher nprod, preferring np
 * if they are equal. */
static struct prefixnode *
prefixnode_maxorfirst(struct prefixnode *np, struct prefixnode *np2)
{
	assert(np != NULL && np2 != NULL);
	return (np->nprod >= np2->nprod) ? np : np2;
}

/* prefixnode_gcf: return the deepest descendant with the highest nprod (or one
 * of the deepest, if there are multiple answering to the description). return
 * NULL if np->nchild == 0 */
static struct prefixnode *
prefixnode_gcf(struct prefixnode *np)
{
	if (np->nchild == 0) {
		return NULL;
	}
	struct prefixnode *gcf = &(struct prefixnode) { .nprod = 0 };
	for (int i = 0; i < np->nchild; i++) {
		struct prefixnode *npci = np->child[i];
		if (npci->nprod > gcf->nprod) {
			struct prefixnode *gcf2 = prefixnode_gcf(npci);
			gcf = gcf2 ? prefixnode_maxorfirst(gcf2, npci) : npci;
		}
	}
	assert(gcf->nprod > 0);
	return gcf;
}

static Prod *
prefixnode_commonprefix(struct prefixnode *np, Symbol *X)
{
	if (np->nprod == 0) {
		return NULL;
	}
	assert(np->nprod <= X->n);
	return prod_subrange(X->prod[np->prod[0]], 0, np->depth);
}

static Prod *
prefixnode_prodtail(struct prefixnode *np, Prod *p)
{
	assert(np->depth <= p->n);
	return prod_subrange(p, np->depth, p->n);
}

#endif
