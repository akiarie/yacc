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

char *
genaction(Action *act, char *reducent)
{
	struct strbuilder *b = strbuilder_create();
	switch (act->type) {
	case ACTION_ACCEPT:
		strbuilder_printf(b,
"			return 1;	/* acc */\n");
		break;
	case ACTION_SHIFT:
		strbuilder_printf(b,
"			%s = %s%d();	/* shift */\n"
"			break;\n", reducent, 
			YYSTATE_NAME, act->u.state);
		break;
	case ACTION_REDUCE:
		strbuilder_printf(b,
"			/* reduce prod %d */\n", act->u.prod);
		break;
	default:
		fprintf(stderr, "invalid actiontype %d\n", act->type);
		exit(EXIT_FAILURE);
	}
	return strbuilder_build(b);
}

void
genstate(FILE *out, int st, struct map *action, struct map *yyterms, char *S)
{
	fprintf(out,
"char *\n"
"%s%d()\n"
"{\n", YYSTATE_NAME, st);
	fprintf(out,
"	char *nt; /* stores reduced nonterminal */\n"
"	int token = yylex();\n"
"	switch (token) {\n");
	bool acc = false;
	for (int i = 0; i < action->n; i++) {
		struct entry e = action->entry[i];
		if (strcmp(e.key, SYMBOL_EOF) == 0) {
			acc = true;
		}
		int termindex = map_getindex(yyterms, e.key);
		if (termindex == -1) { /* nonterminal */
			continue;
		}
		unsigned long termval = (unsigned long) 
			yyterms->entry[termindex].value;
		fprintf(out,
"		case %lu: /* %s */\n", termval, safesym(e.key));
		Action *act = (Action *) map_get(action, e.key);
		fprintf(out, "%s", genaction(act, "nt"));
	}
	fprintf(out,
"		default:\n");
	if (acc) {
		fprintf(out,
"			if (token <= 0) { /* EOF */\n"
"				return \"%s\";\n"
"			}\n", S);
	}
	fprintf(out,
"			fprintf(stderr, \"invalid token '%%d'\", token);\n"
"			exit(EXIT_FAILURE);\n"
"	}\n");
	bool foundnts = false;
	for (int i = 0; i < action->n; i++) { /* nonterminals */
		struct entry e = action->entry[i];
		if (strcmp(e.key, SYMBOL_EOF) == 0) {
			continue;
		}
		if (map_getindex(yyterms, e.key) != -1) { /* terminal */
			continue;
		}
		foundnts = true;
		if (i == 0) {
			fprintf(out,
"	if (strcmp(nt, \"%s\") == 0) {\n", e.key);
		} else {
			fprintf(out,
"	} else if (strcmp(nt, \"%s\") == 0) {\n", e.key);
		}
	}
	if (foundnts) {
		fprintf(out,
"	}\n");
	}
	fprintf(out,
"}\n");
}

void
genstates(FILE *out, Parser P)
{
	fprintf(out, "char ");
	for (int i = 0; i < P.nstate; i++) {
		fprintf(out,
"*%s%d()%s", YYSTATE_NAME, i, (i + 1 < P.nstate ? ",\n\t" : ";\n"));
	}
	fprintf(out, "\n");
	for (int i = 0; i < P.nstate; i++) {
		genstate(out, i, P.action[i], P.yyterms, P.S);
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
"}\n");
}
