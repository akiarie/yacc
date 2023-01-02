#include <stdio.h>
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

void
genparser(FILE *out, Parser P)
{
	fprintf(out,
"/* TODO: parser */\n"
"\n"
"void\n"
"enact(int p)\n"
"{\n"
"	switch(p) {\n");
	for (int i = 1; i < P.prods.n; i++) { /* skip augmented prod 0 */
		char *sym = P.prods.sym[i];
		Prod *p = P.prods.prod[i];
		fprintf(out,
"	case %d: /* %s */\n"
"		%s\n"
"		break;\n", i, genprodstr(sym, p), prepareact(p->action));
	}
	fprintf(out,
"	default:\n"
"		fprintf(stderr, \"invalid reduction on production %%d\\n\", p);\n"
"		exit(EXIT_FAILURE);\n"
"	}\n"
"}\n"
"\n"
"int\n"
"yystatefunc()\n"
"{\n"
"	Action *act = map_get(/* map for this state */, yylex());\n"
"	if (!act) {\n"
"		/* handle error */\n"
"	}\n"
"	switch (act->type) {\n"
"	case ACTION_SHIFT:\n"
"		/* call act->u.state statefunc */;\n"
"	case ACTION_REDUCE: {\n"
"		p = P.prods.prod[act->u.prod]->n;\n"
"		stack_popn(states, p->n);\n"
"		enact(act->u.prod);\n"
"		continue;\n"
"	}\n"
"	case ACTION_ACCEPT:\n"
"		return 1;\n"
"	default:\n"
"		fprintf(stderr, \"unknown action type %%d\\n\", act->type);\n"
"		exit(EXIT_FAILURE);\n"
"	}\n"
"}\n"
"\n");
}

void gen(FILE *out, Parser P)
{
	genparser(out, P);
	fprintf(out,
"Prod *p;\n"
"struct stack states = stack_create();\n"
"stack_push(states, 0);\n"
"while (true) {\n"
"	Action *act = map_get(P.action[stack_top(states)], yylex());\n"
"	if (!act) {\n"
"		/* handle error */\n"
"	}\n"

"}\n"
"\n");
}
