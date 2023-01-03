#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "grammar.h"
#include "parser.h"
#include "gen.h"
#include "util.h"

static char *
safesym(char *sym)
{
	if (strcmp(sym, "\n") == 0) {
		return "\\n";
	}
	return sym;
}

static char *
genprodstr(char *sym, Prod *p)
{
	struct strbuilder *b = strbuilder_create();
	strbuilder_printf(b, "%s -> ", sym);
	for (int j = 0; j < p->n; j++) {
		strbuilder_printf(b, "%s%s", safesym(p->sym[j]),
			(j + 1 < p->n) ? " " : "" /* spacing */);
	}
	return strbuilder_build(b);
}

char *
prepareact(char *action)
{
	struct strbuilder *b = strbuilder_create();
	if (action) {
		strbuilder_printf(b, action);
	}
	return strbuilder_build(b);
}

static char *
prod_safestr(Prod *p)
{
	struct strbuilder *b = strbuilder_create();
	for (int i = 0; i < p->n; i++) {
		strbuilder_printf(b, "%s%s", safesym(p->sym[i]),
			(i + 1 < p->n) ? " " : "" /* spacing */);
	}
	return strbuilder_build(b);
}

char *
genreduce(char *reducent, char *prefix, char *sym, Prod *p)
{
	struct strbuilder *b = strbuilder_create();
	char *prodstr = prod_safestr(p);
	strbuilder_printf(b,
"%s/* reduce %s -> %s */\n", prefix, sym, prodstr);
	free(prodstr);
	strbuilder_printf(b,
"%s%s = (struct parseresult) { .nt = \"%s\", .nret = %d };\n", prefix, reducent, 
		sym, p->n);
	return strbuilder_build(b);
}

char *
genaction(Action *act, char *reducent, char *prefix, struct lrprodset prods)
{
	struct strbuilder *b = strbuilder_create();
	switch (act->type) {
	case ACTION_ACCEPT:
		strbuilder_printf(b, "%s/* acc */\n", prefix);
		strbuilder_puts(b, genreduce(reducent, prefix, prods.sym[0],
			prods.prod[0]));
		break;
	case ACTION_SHIFT:
		strbuilder_printf(b,
"%s%s = %s%d(symbol_yylex());\n", prefix, reducent, YYSTATE_NAME, act->u.state);
		break;
	case ACTION_REDUCE:
		strbuilder_puts(b, genreduce(reducent, prefix,
			prods.sym[act->u.prod], prods.prod[act->u.prod]));
		break;
	default:
		fprintf(stderr, "invalid actiontype %d\n", act->type);
		exit(EXIT_FAILURE);
	}
	return strbuilder_build(b);
}

void
gentokenparse(FILE *out, Parser P, int state)
{
	bool acc = false;
	fprintf(out,
"		switch (token) {\n");
	for (int i = 0; i < P.action[state]->n; i++) {
		struct entry e = P.action[state]->entry[i];
		if (strcmp(e.key, SYMBOL_EOF) == 0) {
			acc = true;
		}
		int termindex = map_getindex(P.yyterms, e.key);
		if (termindex == -1) { /* nonterminal */
			continue;
		}
		unsigned long termval = (unsigned long) 
			P.yyterms->entry[termindex].value;
		fprintf(out,
"		case %lu: /* %s */\n", termval, safesym(e.key));
		Action *act = (Action *) map_get(P.action[state], e.key);
		fprintf(out, "%s", genaction(act, "r", 
"			", P.prods));
		fprintf(out,
"			break;\n");
	}
	fprintf(out,
"		default:\n");
	if (acc) {
		fprintf(out,
"			if (token <= 0) { /* EOF */\n");
		fprintf(out, "%s", genaction(action_accept(), "r",
"				", P.prods));
		fprintf(out,
"			}\n");
	}
		fputs(
"			fprintf(stderr, \"invalid token '%d'\", token);\n"
"			exit(EXIT_FAILURE);\n", out);
	fprintf(out,
"		}\n");
}

void
genntparse(FILE *out, Parser P, int state)
{
	bool found = false;
	for (int i = 0; i < P.action[state]->n; i++) { /* nonterminals */
		struct entry e = P.action[state]->entry[i];
		if (strcmp(e.key, SYMBOL_EOF) == 0) {
			continue;
		}
		if (map_getindex(P.yyterms, e.key) != -1) { /* terminal */
			continue;
		}
		found = true;
		if (i == 0) {
			fprintf(out,
"		if (strcmp(nt, \"%s\") == 0) {\n", e.key);
		} else {
			fprintf(out,
"		} else if (strcmp(nt, \"%s\") == 0) {\n", e.key);
		}
		Action *act = (Action *) map_get(P.action[state], e.key);
		assert(act->type == ACTION_SHIFT);
		fprintf(out, "%s", genaction(act, "r",
"			", P.prods));
	}
	if (found) {
	fputs(
"		} else {\n"
"			fprintf(stderr, \"invalid nonterminal '%s'\", nt);\n"
"			exit(EXIT_FAILURE);\n"
"		}\n", out);
	}
}

void
genstate(FILE *out, Parser P, int state)
{
	fprintf(out,
"struct parseresult\n"
"%s%d(struct symbol s)\n"
"{\n", YYSTATE_NAME, state);
	fprintf(out,
"	struct parseresult r;\n"
"	if (s.terminal) {\n"
"		int token = s.u.token;\n");
	gentokenparse(out, P, state);
	fprintf(out,
"	} else {\n"
"		char *nt = s.u.nt;\n");
	genntparse(out, P, state);
	fprintf(out,
"	}\n"
"	/* pop stack until no more returns and then recurse on the state\n"
"	 * where we land */\n"
"	if (r.nret > 0) {\n"
"		return (struct parseresult) {\n"
"			.nt = r.nt, .nret = r.nret - 1,\n"
"		};\n"
"	}\n"
"	return %s%d(symbol_nt(r.nt));\n", YYSTATE_NAME, state);
	fprintf(out,
"}\n");
}

void
genstates(FILE *out, Parser P)
{
	fprintf(out, 
"struct parseresult {\n"
"	char *nt;	/* production head */\n"
"	size_t nret;	/* remaining returns */\n"
"};\n"
"\n"
"struct symbol {\n"
"	union symbolval {\n"
"		int token;\n"
"		char *nt;\n"
"	} u;\n"
"	bool terminal;\n"
"};\n"
"\n"
"struct symbol\n"
"symbol_yylex()\n"
"{\n"
"	return (struct symbol) {\n"
"		.u = (union symbolval) { .token = yylex() },\n"
"		.terminal = true,\n"
"	};\n"
"}\n"
"\n"
"struct symbol\n"
"symbol_nt(char *nt)\n"
"{\n"
"	return (struct symbol) {\n"
"		.u = (union symbolval) { .nt = nt },\n"
"		.terminal = false,\n"
"	};\n"
"}\n"
"\n"
"struct parseresult\n");
	for (int i = 0; i < P.nstate; i++) {
		fprintf(out,
"%s%d(struct symbol)%s", YYSTATE_NAME, i, (i + 1 < P.nstate ? ",\n\t" : ";\n"));
	}
	fprintf(out, "\n");
	for (int i = 0; i < P.nstate; i++) {
		genstate(out, P, i);
		fprintf(out, "\n");
	}
}

void
gen(FILE *out, Parser P)
{
	genstates(out, P);
	fprintf(out,
"int\n"
"yyparse()\n"
"{\n"
"	struct parseresult r = %s0(symbol_yylex());\n", YYSTATE_NAME);
	fprintf(out,
"}\n");
}
