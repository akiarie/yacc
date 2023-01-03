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
genaction(Action *act, char *reducent, char *prefix)
{
	struct strbuilder *b = strbuilder_create();
	switch (act->type) {
	case ACTION_ACCEPT:
		strbuilder_printf(b, 
"%sreturn 1;	/* acc */\n", prefix);
		break;
	case ACTION_SHIFT:
		strbuilder_printf(b,
"%s%s = %s%d();	/* shift */\n", prefix, reducent, YYSTATE_NAME, act->u.state);
		break;
	case ACTION_REDUCE:
		strbuilder_printf(b,
"%s/* reduce prod %d */\n", prefix, act->u.prod);
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
"struct parseresult\n"
"%s%d()\n"
"{\n", YYSTATE_NAME, st);
	fprintf(out,
"	struct parseresult r;\n"
"	int token = yylex();\n"
"	switch (token) {\n");
	bool acc = false;
	bool foundnts = false;
	for (int i = 0; i < action->n; i++) {
		struct entry e = action->entry[i];
		if (strcmp(e.key, SYMBOL_EOF) == 0) {
			acc = true;
		}
		int termindex = map_getindex(yyterms, e.key);
		if (termindex == -1) { /* nonterminal */
			foundnts = true;
			continue;
		}
		unsigned long termval = (unsigned long) 
			yyterms->entry[termindex].value;
		fprintf(out,
"		case %lu: /* %s */\n", termval, safesym(e.key));
		Action *act = (Action *) map_get(action, e.key);
		fprintf(out, "%s", genaction(act, "r", 
"			"));
		fprintf(out,
"			break;\n");
	}
	if (acc) {
	fprintf(out,
"		default:\n");
		fprintf(out,
"			if (token <= 0) { /* EOF */\n");
		fprintf(out, "%s", genaction(action_accept(), "r",
"				"));
		fprintf(out,
"			}\n");
	}
	fprintf(out,
"	}\n");
	if (foundnts && !acc) {
		fprintf(out,
"	if (r.nret > 0) {\n"
"		return (struct parseresult) {.nt = r.nt, .nret = r.nret - 1};\n"
"	}\n"
"	return %snt%d(r.nt);\n", YYSTATE_NAME, st);
		fprintf(out,
"}\n"
"\n"
"struct parseresult\n"
"%snt%d(char *nt)\n"
"{\n", YYSTATE_NAME, st);
		fprintf(out,
"	struct parseresult r;\n");
		for (int i = 0; i < action->n; i++) { /* nonterminals */
			struct entry e = action->entry[i];
			if (strcmp(e.key, SYMBOL_EOF) == 0) {
				continue;
			}
			if (map_getindex(yyterms, e.key) != -1) { /* terminal */
				continue;
			}
			if (i == 0) {
				fprintf(out,
"	if (strcmp(nt, \"%s\") == 0) {\n", e.key);
			} else {
				fprintf(out,
"	} else if (strcmp(nt, \"%s\") == 0) {\n", e.key);
			}
			Action *act = (Action *) map_get(action, e.key);
			assert(act->type == ACTION_SHIFT);
			fprintf(out, "%s", genaction(act, "r",
"		"));
		}
			fputs(
"	} else {\n"
"		fprintf(stderr, \"invalid nonterminal '%s'\", nt);\n"
"		exit(EXIT_FAILURE);\n"
"	}\n", out);
		fprintf(out,
"	if (r.nret > 0) {\n"
"		return (struct parseresult) {.nt = r.nt, .nret = r.nret - 1};\n"
"	}\n"
"	return %snt%d(r.nt);\n", YYSTATE_NAME, st);
	} else {
		fprintf(out,
"	fprintf(stderr, \"invalid token '%%d'\", token);\n"
"	exit(EXIT_FAILURE);\n");
	}
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
"struct parseresult\n");
	for (int i = 0; i < P.nstate; i++) {
		fprintf(out,
"%s%d(), %snt%d(char *)%s", YYSTATE_NAME, i, YYSTATE_NAME, i, 
			(i + 1 < P.nstate ? ",\n\t" : ";\n"));
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
