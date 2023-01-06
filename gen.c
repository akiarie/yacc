#include <stdio.h>
#include <assert.h>
#include <string.h>

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
"typedef struct yystack {\n"
"	struct yystack *next;\n"
"	int val;\n"
"} YYStack;\n"
"\n"
"YYStack *\n"
"yystack_create(int val)\n"
"{\n"
"	YYStack *stack = calloc(1, sizeof(YYStack));\n"
"	stack->val = val;\n"
"	return stack;\n"
"}\n"
"\n"
"void\n"
"yystack_destroy(YYStack *stack)\n"
"{\n"
"	if (stack->next) {\n"
"		yystack_destroy(stack->next);\n"
"	}\n"
"	free(stack);\n"
"}\n"
"\n"
"void\n"
"yystack_push(YYStack *stack, int val)\n"
"{\n"
"	stack->next = yystack_create(val);\n"
"}\n"
"\n"
"YYStack *\n"
"yystack_popn(YYStack *stack, int n)\n"
"{\n"
"	for (int i = 0; i < n; i++) {\n"
"		YYStack *last = stack;\n"
"		stack = stack->next;\n"
"		assert(stack);\n"
"		yystack_destroy(last);\n"
"	}\n"
"	return stack;\n"
"}\n"
"\n"
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

char *
genaction(Action *act, struct lrprodset prods)
{
	struct strbuilder *b = strbuilder_create();
	char *reduce;
	switch (act->type) {
	case ACTION_SHIFT:
		strbuilder_printf(b, "yyaction_shift(%d);", act->u.state);
		break;
	case ACTION_REDUCE:
		reduce = genreduce(prods, act->u.prod);
		strbuilder_puts(b, reduce);
		free(reduce);
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
	bool eof = false;
	fprintf(out,
"%sswitch (token) {\n", prefix);
	for (int i = 0; i < P.action[state]->n; i++) {
		struct entry e = P.action[state]->entry[i];
		if (strcmp(e.key, SYMBOL_EOF) == 0) {
			eof = true;
			continue;
		}
		/* only output cases for (non-accepting) literals and yyterms */
		if (isliteral(e.key)) {
			fprintf(out,
"%s	case '%s':\n", prefix, e.key);
		} else {
			if (map_getindex(P.yyterms, e.key) == -1) { /* nonterminal */
				continue;
			}
			fprintf(out,
"%s	case %s:\n", prefix, e.key);
		}
		char *action = genaction(map_get(P.action[state], e.key), P.prods);
		fprintf(out, 
"%s		return %s\n", prefix, action);
		free(action);
	}
	fprintf(out,
"%s	default:\n", prefix);
	if (eof) {
		fprintf(out,
"%s		if (token <= 0) {\n"
"%s			return yyaction_accept();\n"
"%s		}\n", prefix, prefix, prefix);
	}
	fprintf(out,
"%s		fprintf(stderr, \"invalid token %%d in state %%d\\n\", token, state);\n"
"%s		exit(EXIT_FAILURE);\n"
"%s}\n", prefix, prefix, prefix);
}

void
genstategoto(FILE *out, Parser P, int state, char *prefix)
{
	bool found = false;
	for (int i = 0; i < P.action[state]->n; i++) {
		struct entry e = P.action[state]->entry[i];
		/* nonterminals only */
		if (isliteral(e.key) || map_getindex(P.yyterms, e.key) != -1 ||
				strcmp(e.key, SYMBOL_EOF) == 0) {
			continue;
		}
		found = true;
		/* prefix with conditional else */
		fprintf(out,
"%s%s", prefix, (i > 0 ? "} else ": ""));
		fprintf(out,
		"if (strcmp(nt, \"%s\") == 0) {\n", e.key);
		Action *act = (Action *) map_get(P.action[state], e.key);
		assert(act->type == ACTION_SHIFT);
		fprintf(out,
"%s	return %d;\n", prefix, act->u.state);
	}
	if (found) {
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
	gentokens(out, P.yyterms);
	fprintf(out,
"\n");
	gentabletypes(out);
	fprintf(out, 
"\n"
"YYAction\n"
"%s(int state, int token)\n", YY_STATE_ACTION);
	fprintf(out,
"{\n"
"	switch (state) {\n");
	for (int i = 0; i < P.nstate; i++) {
		fprintf(out,
"		case %d:\n", i);
		genstateaction(out, P, i,
"			");
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
"		case %d:\n", i);
		genstategoto(out, P, i,
"			");
	}
	fprintf(out,
"	}\n"
"	assert(false); /* unknown state */\n"
"}\n");
}

void
gen(FILE *out, Parser P)
{
	fprintf(out, "%s", P.precode);
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
"	YYStack *states = yystack_create(0);\n"
"	int token = yylex();\n"
"	while (1) {\n");
	fprintf(out,
"		YYAction act = %s(states->val, token);\n", YY_STATE_ACTION);
	fprintf(out,
"		switch (act.type) {\n"
"			case YYACTION_SHIFT:\n"
"				yystack_push(states, act.u.state);\n"
"				token = yylex();\n"
"			case YYACTION_REDUCE:\n"
"				yystack_popn(states, act.u.r.len);\n"
"				yystack_push(states, %s(states->val, act.u.r.nt));\n",
		YY_STATE_GOTO);
	fprintf(out,
"			case YYACTION_ACCEPT:\n"
"				yystack_destroy(states);\n"
"				return 0;\n"
"		}\n"
"	}\n"
"}\n");
	fprintf(out, "%s", P.postcode);
}
