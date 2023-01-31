#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "grammar.h"
#include "parser.h"
#include "gen.h"
#include "util.h"

void
gentokens(FILE *out, struct map *yyterms)
{
	fprintf(out,
"/* TOKEN DEFINITIONS */\n");
	for (int i = 0; i < yyterms->n; i++) {
		struct entry e = yyterms->entry[i];
		fprintf(out,
"#define %s %lu\n", e.key, (unsigned long) e.value);
	}
}

void
gentabletypes(FILE *out)
{
	fprintf(out,
"#define DEFAULT_CAP 100\n"
"#define CAP_MULT 2\n"
"\n"
"typedef struct yystack {\n"
"	int *val;\n"
"	size_t len, cap;\n"
"} YYStack;\n"
"\n"
"void\n"
"yystack_alloc(YYStack *stack, size_t len)\n"
"{\n"
"	assert(len >= 0 && stack->cap > 0);\n"
"	while (stack->cap <= len) {\n"
"		stack->cap *= CAP_MULT;\n"
"	}\n"
"	stack->val = realloc(stack->val, sizeof(int) * stack->cap);\n"
"	stack->len = len;\n"
"}\n"
"\n"
"YYStack *\n"
"yystack_create(int val)\n"
"{\n"
"	YYStack *stack = calloc(1, sizeof(YYStack));\n"
"	stack->cap = DEFAULT_CAP;\n"
"	stack->val = malloc(sizeof(int) * stack->cap);\n"
"	stack->val[0] = val;\n"
"	stack->len = 1;\n"
"	return stack;\n"
"}\n"
"\n"
"void\n"
"yystack_destroy(YYStack *stack)\n"
"{\n"
"	free(stack->val);\n"
"	free(stack);\n"
"}\n"
"\n"
"void\n"
"yystack_push(YYStack *stack, int val)\n"
"{\n"
"	int index = stack->len;\n"
"	yystack_alloc(stack, stack->len + 1);\n"
"	/* stack->len increases above */\n"
"	stack->val[index] = val;\n"
"}\n"
"\n"
"void\n"
"yystack_popn(YYStack *stack, int n)\n"
"{\n"
"	assert(n < stack->len);\n"
"	for (int i = 0; i < n; i++) {\n"
"		stack->len--;\n"
"	}\n"
"}\n"
"\n"
"int\n"
"yystack_top(YYStack *stack)\n"
"{\n"
"	assert(stack->len > 0);\n"
"	return stack->val[stack->len - 1];\n"
"}\n"
"\n"
"int\n"
"yystack_1n(YYStack *stack, int n)\n"
"{\n"
"	assert(0 < n && n <= stack->len);\n"
"	return stack->val[stack->len - n];\n"
"}\n"
"\n"
"enum yyactiontype {\n"
"	YYACTION_ACCEPT	= 1 << 0,\n"
"	YYACTION_SHIFT	= 1 << 1,\n"
"	YYACTION_REDUCE	= 1 << 2,\n"
"};\n"
"\n"
"typedef struct {\n"
"	enum yyactiontype type;\n"
"	union yyactunion {\n"
"		int state;	/* shift */\n"
"		struct yyreduce {\n"
"			char *nt;	/* nonterminal */\n"
"			size_t len;	/* number of symbols */\n"
"		} r;\n"
"	} u;\n"
"} YYAction;\n"
"\n"
"YYAction\n"
"yyaction_accept()\n"
"{\n"
"	return (YYAction) { YYACTION_ACCEPT };\n"
"}\n"
"\n"
"YYAction\n"
"yyaction_shift(int st)\n"
"{\n"
"	return (YYAction) { YYACTION_SHIFT, (union yyactunion) { .state = st } };\n"
"}\n"
"\n"
"YYAction\n"
"yyaction_reduce(char *nt, size_t len)\n"
"{\n"
"	return (YYAction) {\n"
"		YYACTION_REDUCE, (union yyactunion) {\n"
"			.r = (struct yyreduce) { nt, len }\n"
"		}\n"
"	};\n"
"}\n");
}

static char *
prod_safestr(Prod *p)
{
	struct strbuilder *b = strbuilder_create();
	for (int i = 0; i < p->n; i++) {
		strbuilder_printf(b, "%s%s", p->sym[i],
			(i + 1 < p->n) ? " " : "" /* spacing */);
	}
	return strbuilder_build(b);
}

char *
genreduce(struct lrprodset prods, int k)
{
	struct strbuilder *b = strbuilder_create();
	char *prodstr = prod_safestr(prods.prod[k]);
	char *sym = prods.sym[k];
	strbuilder_printf(b, "yyaction_reduce(\"%s\", %lu); /* %s -> %s */", sym,
		prods.prod[k]->n, sym, prodstr);
	free(prodstr);
	return strbuilder_build(b);
}

struct numparse {
	int num;
	size_t len;
};

struct numparse
parsenum(char *s)
{
	char *t;
	for (t = s; isdigit(*t); t++) {}
	size_t len = t - s + 1;
	char *u = malloc(sizeof(char) * len);
	snprintf(u, len, "%s", s);
	int num = atoi(u);
	free(u);
	return (struct numparse) {num, t - s};
}

char *
translateact(char *s, size_t len)
{
	struct strbuilder *b = strbuilder_create();
	struct numparse np;
	while (*s) {
		if (*s != '$') {
			strbuilder_putc(b, *s++);
			continue;
		}
		switch (*++s) {
		case '$':
			strbuilder_printf(b, "%s", YY_REDUCE_VAL);
			s++;
			continue;
		default:
			assert('0' <= *s && *s <= '9');
			np = parsenum(s);
			s += np.len;
			assert((len + 1) > np.num);
			strbuilder_printf(b, "yystack_1n(values, %d)", (len + 1) - (np.num));
		}
	}
	return strbuilder_build(b);
}

char *
genactwithdef(char *action)
{
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "$$ = $1;");
	if (action) {
		strbuilder_printf(b, " %s", action);
	}
	return strbuilder_build(b);
}

char *
genvalues(char *action, char *prefix, size_t len)
{
	assert(action && len > 0);
	struct strbuilder *b = strbuilder_create();
	char *actwdef = genactwithdef(action);
	char *s = translateact(actwdef, len);
	free(actwdef);
	if (strlen(action) > 0) {
	strbuilder_printf(b,
"%s/* action %s */\n", prefix, action);
	}
	strbuilder_printf(b,
"%s%s\n", prefix, s);
	free(s);
	return strbuilder_build(b);
}

char *
genaction(Action *act, struct lrprodset prods, char *prefix)
{
	struct strbuilder *b = strbuilder_create();
	char *reduce, *vals;
	size_t len;
	switch (act->type) {
	case ACTION_SHIFT:
		strbuilder_printf(b, 
"%sreturn yyaction_shift(%d);\n", prefix, act->u.state);
		break;
	case ACTION_REDUCE:
		if ((len = prods.prod[act->u.prod]->n)) {
			vals = genvalues(prods.prod[act->u.prod]->action, prefix, len);
			strbuilder_printf(b, "%s", vals);
			free(vals);
			strbuilder_printf(b, 
	"%syystack_popn(values, %lu);\n", prefix, len);
			strbuilder_printf(b, 
	"%syystack_push(values, val);\n", prefix);
		} else {
			if (prods.prod[act->u.prod]->action) {
			strbuilder_printf(b, 
	"%s%s\n", prefix, prods.prod[act->u.prod]->action);
			}
			strbuilder_printf(b, 
	"%syystack_push(values, 0);\n", prefix);
		}
		reduce = genreduce(prods, act->u.prod);
		strbuilder_printf(b, 
"%sreturn %s\n", prefix, reduce);
		free(reduce);
		break;
	case ACTION_ACCEPT:
		if ((len = prods.prod[act->u.prod]->n)) {
			vals = genvalues(prods.prod[act->u.prod]->action, prefix, len);
			strbuilder_printf(b, "%s", vals);
			free(vals);
		}
		strbuilder_printf(b, 
"%sreturn yyaction_accept();\n", prefix);
		break;
	default:
		fprintf(stderr, "invalid actiontype %d\n", act->type);
		exit(EXIT_FAILURE);
	}
	return strbuilder_build(b);
}

void
genstateaction(FILE *out, Parser P, int state, char *prefix)
{
	char *eofkey = NULL;
	fprintf(out,
"%sswitch (token) {\n", prefix);
	for (int i = 0; i < P.action[state]->n; i++) {
		struct entry e = P.action[state]->entry[i];
		if (strcmp(e.key, SYMBOL_EOF) == 0) {
			eofkey = e.key;
			continue;
		}
		/* only output cases for (non-accepting) literals and yyterms */
		if (isliteral(e.key)) {
			fprintf(out,
"%scase '%s':\n", prefix, e.key);
		} else {
			if (map_getindex(P.yyterms, e.key) == -1) { /* nonterminal */
				continue;
			}
			fprintf(out,
"%scase %s:\n", prefix, e.key);
		}
		struct strbuilder *b = strbuilder_create();
		strbuilder_printf(b,
"%s	", prefix);
		char *subprefix = strbuilder_build(b);
		char *action = genaction(map_get(P.action[state], e.key),
			P.prods, subprefix);
		free(subprefix);
		fprintf(out, "%s", action);
		free(action);
	}
	fprintf(out,
"%sdefault:\n", prefix);
	if (eofkey) {
		fprintf(out,
"%s	if (token <= 0) {\n", prefix);
		struct strbuilder *b = strbuilder_create();
		strbuilder_printf(b,
"%s		", prefix);
		char *subprefix = strbuilder_build(b);
		char *action = genaction(map_get(P.action[state], eofkey),
			P.prods, subprefix);
		free(subprefix);
		fprintf(out, "%s", action);
		fprintf(out,
"%s	}\n", prefix);
	}
	fprintf(out,
"%s	fprintf(stderr, \"invalid token %%d in state %%d\\n\", token, state);\n"
"%s	exit(EXIT_FAILURE);\n"
"%s}\n", prefix, prefix, prefix);
}

void
genstategoto(FILE *out, Parser P, int state, char *prefix)
{
	int found = 0;
	for (int i = 0; i < P.action[state]->n; i++) {
		struct entry e = P.action[state]->entry[i];
		/* nonterminals only */
		if (isliteral(e.key) || map_getindex(P.yyterms, e.key) != -1 ||
				strcmp(e.key, SYMBOL_EOF) == 0) {
			continue;
		}
		found++;
		/* prefix with conditional else */
		fprintf(out,
"%s%s", prefix, (found > 1 ? "} else ": ""));
		fprintf(out,
		"if (strcmp(nt, \"%s\") == 0) {\n", e.key);
		Action *act = (Action *) map_get(P.action[state], e.key);
		assert(act->type == ACTION_SHIFT);
		fprintf(out,
"%s	return %d;\n", prefix, act->u.state);
	}
	if (found > 0) {
		fprintf(out,
"%s}\n", prefix);
	}
	fprintf(out,
"%sfprintf(stderr, \"invalid reduction '%%s' in state %%d\\n\", nt, state);\n"
"%sexit(EXIT_FAILURE);\n", prefix, prefix);
}

void
gentable(FILE *out, Parser P)
{
	gentabletypes(out);
	fprintf(out, 
"\n"
"YYAction\n"
"%s(int state, int token, YYStack *values)\n", YY_STATE_ACTION);
	fprintf(out,
"{\n"
"	int %s;\n", YY_REDUCE_VAL);
	fprintf(out,
"	switch (state) {\n");
	for (int i = 0; i < P.nstate; i++) {
		fprintf(out,
"	case %d:\n", i);
		genstateaction(out, P, i,
"		");
	}
	fprintf(out,
"	}\n"
"	assert(false); /* unknown state */\n"
"}\n");

	fprintf(out, 
"\n"
"int\n"
"%s(int state, char *nt)\n", YY_STATE_GOTO);
	fprintf(out,
"{\n"
"	switch (state) {\n");
	for (int i = 0; i < P.nstate; i++) {
		fprintf(out,
"	case %d:\n", i);
		genstategoto(out, P, i,
"		");
	}
	fprintf(out,
"	}\n"
"	assert(false); /* unknown state */\n"
"}\n");
}

void
gen_headers(FILE *out, Parser P)
{
	fprintf(out,
"#ifndef YY_TOKENS\n"
"#define YY_TOKENS\n"
"\n"
"int\n"
"yyparse();\n"
"\n");
	gentokens(out, P.yyterms);
	fprintf(out,
"\n"
"#endif\n");
}

void
gen(FILE *out, Parser P)
{
	gen_headers(out, P);
	fprintf(out,
"\n"
"%s", P.precode);
	fprintf(out,
"\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <stdbool.h>\n"
"#include <assert.h>\n"
"#include <string.h>\n"
"\n"
"int\n"
"yylex();\n"
"\n");
	gentable(out, P);
	fprintf(out,
"\n"
"int\n"
"yyparse()\n"
"{\n"
"	int token = yylex();\n"
"	YYStack *values = yystack_create(yylval);\n"
"	YYStack *states = yystack_create(0);\n"
"	while (true) {\n");
	fprintf(out,
"		YYAction act = %s(yystack_top(states), token, values);\n",
		YY_STATE_ACTION);
	fprintf(out,
"		switch (act.type) {\n"
"		case YYACTION_SHIFT:\n"
"			yystack_push(states, act.u.state);\n"
"			token = yylex();\n"
"			yystack_push(values, yylval);\n"
"			continue;\n"
"		case YYACTION_REDUCE:\n"
"			yystack_popn(states, act.u.r.len);\n"
"			yystack_push(states, %s(yystack_top(states), act.u.r.nt));\n"
"			continue;\n",
		YY_STATE_GOTO);
	fprintf(out,
"		case YYACTION_ACCEPT:\n"
"			yystack_destroy(states);\n"
"			yystack_destroy(values);\n"
"			return 0;\n"
"		}\n"
"		assert(false); /* invalid action type */\n"
"	}\n"
"}\n");
	fprintf(out, "%s", P.postcode);
}
