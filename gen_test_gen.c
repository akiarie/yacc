#include <ctype.h>

int yylval;

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

int
yylex();

/* TOKEN DEFINITIONS */
#define DIGIT 257

typedef struct yystack {
	struct yystack *next;
	int val;
} YYStack;

YYStack *
yystack_create(int val)
{
	YYStack *stack = calloc(1, sizeof(YYStack));
	stack->val = val;
	return stack;
}

void
yystack_destroy(YYStack *stack)
{
	if (stack->next) {
		yystack_destroy(stack->next);
	}
	free(stack);
}

void
yystack_push(YYStack *stack, int val)
{
	stack->next = yystack_create(val);
}

YYStack *
yystack_popn(YYStack *stack, int n)
{
	for (int i = 0; i < n; i++) {
		YYStack *last = stack;
		stack = stack->next;
		assert(stack);
		yystack_destroy(last);
	}
	return stack;
}


enum yyactiontype {
	YYACTION_ACCEPT	= 1 << 0,
	YYACTION_SHIFT	= 1 << 1,
	YYACTION_REDUCE	= 1 << 2,
};

typedef struct {
	enum yyactiontype type;
	union yyactunion {
		int state;	/* shift */
		struct yyreduce {
			char *nt;	/* nonterminal */
			size_t len;	/* number of symbols */
		} r;
	} u;
} YYAction;

YYAction
yyaction_accept()
{
	return (YYAction) { YYACTION_ACCEPT };
}

YYAction
yyaction_shift(int st)
{
	return (YYAction) { YYACTION_SHIFT, (union yyactunion) { .state = st } };
}

YYAction
yyaction_reduce(char *nt, size_t len)
{
	return (YYAction) {
		YYACTION_REDUCE, (union yyactunion) {
			.r = (struct yyreduce) { nt, len }
		}
	};
}

YYAction
yystaction(int state, int token)
{
	switch (state) {
		case 0:
			switch (token) {
				case '(':
					return yyaction_shift(5);
				case DIGIT:
					return yyaction_shift(6);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 1:
			switch (token) {
				default:
					if (token <= 0) {
						return yyaction_accept();
					}
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 2:
			switch (token) {
				case '\n':
					return yyaction_shift(7);
				case '+':
					return yyaction_shift(8);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 3:
			switch (token) {
				case '\n':
					return yyaction_reduce("expr", 1); /* expr -> term */
				case '+':
					return yyaction_reduce("expr", 1); /* expr -> term */
				case ')':
					return yyaction_reduce("expr", 1); /* expr -> term */
				case '*':
					return yyaction_shift(9);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 4:
			switch (token) {
				case '\n':
					return yyaction_reduce("term", 1); /* term -> factor */
				case '+':
					return yyaction_reduce("term", 1); /* term -> factor */
				case ')':
					return yyaction_reduce("term", 1); /* term -> factor */
				case '*':
					return yyaction_reduce("term", 1); /* term -> factor */
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 5:
			switch (token) {
				case '(':
					return yyaction_shift(5);
				case DIGIT:
					return yyaction_shift(6);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 6:
			switch (token) {
				case '\n':
					return yyaction_reduce("factor", 1); /* factor -> DIGIT */
				case '+':
					return yyaction_reduce("factor", 1); /* factor -> DIGIT */
				case ')':
					return yyaction_reduce("factor", 1); /* factor -> DIGIT */
				case '*':
					return yyaction_reduce("factor", 1); /* factor -> DIGIT */
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 7:
			switch (token) {
				default:
					if (token <= 0) {
						return yyaction_accept();
					}
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 8:
			switch (token) {
				case '(':
					return yyaction_shift(5);
				case DIGIT:
					return yyaction_shift(6);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 9:
			switch (token) {
				case '(':
					return yyaction_shift(5);
				case DIGIT:
					return yyaction_shift(6);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 10:
			switch (token) {
				case ')':
					return yyaction_shift(13);
				case '+':
					return yyaction_shift(8);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 11:
			switch (token) {
				case '\n':
					return yyaction_reduce("expr", 3); /* expr -> expr + term */
				case '+':
					return yyaction_reduce("expr", 3); /* expr -> expr + term */
				case ')':
					return yyaction_reduce("expr", 3); /* expr -> expr + term */
				case '*':
					return yyaction_shift(9);
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 12:
			switch (token) {
				case '\n':
					return yyaction_reduce("term", 3); /* term -> term * factor */
				case '+':
					return yyaction_reduce("term", 3); /* term -> term * factor */
				case ')':
					return yyaction_reduce("term", 3); /* term -> term * factor */
				case '*':
					return yyaction_reduce("term", 3); /* term -> term * factor */
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
		case 13:
			switch (token) {
				case '\n':
					return yyaction_reduce("factor", 3); /* factor -> ( expr ) */
				case '+':
					return yyaction_reduce("factor", 3); /* factor -> ( expr ) */
				case ')':
					return yyaction_reduce("factor", 3); /* factor -> ( expr ) */
				case '*':
					return yyaction_reduce("factor", 3); /* factor -> ( expr ) */
				default:
					fprintf(stderr, "invalid token %d in state %d\n", token, state);
					exit(EXIT_FAILURE);
			}
	}
	assert(false); /* unknown state */
}

int
yystgoto(int state, char *nt)
{
	switch (state) {
		case 0:
			if (strcmp(nt, "line") == 0) {
				return 1;
			} else if (strcmp(nt, "expr") == 0) {
				return 2;
			} else if (strcmp(nt, "term") == 0) {
				return 3;
			} else if (strcmp(nt, "factor") == 0) {
				return 4;
			}
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 1:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 2:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 3:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 4:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 5:
			if (strcmp(nt, "expr") == 0) {
				return 10;
			} else if (strcmp(nt, "term") == 0) {
				return 3;
			} else if (strcmp(nt, "factor") == 0) {
				return 4;
			}
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 6:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 7:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 8:
			if (strcmp(nt, "term") == 0) {
				return 11;
			} else if (strcmp(nt, "factor") == 0) {
				return 4;
			}
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 9:
			if (strcmp(nt, "factor") == 0) {
				return 12;
			}
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 10:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 11:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 12:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
		case 13:
			fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
			exit(EXIT_FAILURE);
	}
	assert(false); /* unknown state */
}

int
yyparse()
{
	YYStack *states = yystack_create(0);
	int token = yylex();
	while (1) {
		YYAction act = yystaction(states->val, token);
		switch (act.type) {
			case YYACTION_SHIFT:
				yystack_push(states, act.u.state);
				token = yylex();
			case YYACTION_REDUCE:
				yystack_popn(states, act.u.r.len);
				yystack_push(states, yystgoto(states->val, act.u.r.nt));
			case YYACTION_ACCEPT:
				yystack_destroy(states);
				return 0;
		}
	}
}

int
yylex()
{
	int c = getchar();
	if (isdigit(c)) {
		yylval = c - '0';
		return DIGIT;
	}
	return c;
}

int
main()
{
	return yyparse();
}
