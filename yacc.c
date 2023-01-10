#ifndef YY_TOKENS
#define YY_TOKENS

int
yyparse();

/* TOKEN DEFINITIONS */
#define MARK 257
#define START 258
#define IDENTIFIER 259
#define UNION 260
#define LCURL 261
#define RCURL 262
#define TOKEN 263
#define LEFT 264
#define RIGHT 265
#define NONASSOC 266
#define TYPE 267
#define NUMBER 268
#define C_IDENTIFIER 269
#define PREC 270

#endif

#include "lex.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

int
yylex();

#define DEFAULT_CAP 100
#define CAP_MULT 2

typedef struct yystack {
	int *val;
	size_t len, cap;
} YYStack;

void
yystack_alloc(YYStack *stack, size_t len)
{
	assert(len >= 0 && stack->cap > 0);
	while (stack->cap <= len) {
		stack->cap *= CAP_MULT;
	}
	stack->val = realloc(stack->val, sizeof(int) * stack->cap);
	stack->len = len;
}

YYStack *
yystack_create(int val)
{
	YYStack *stack = calloc(1, sizeof(YYStack));
	stack->cap = DEFAULT_CAP;
	stack->val = malloc(sizeof(int) * stack->cap);
	stack->val[0] = val;
	stack->len = 1;
	return stack;
}

void
yystack_destroy(YYStack *stack)
{
	free(stack->val);
	free(stack);
}

void
yystack_push(YYStack *stack, int val)
{
	int index = stack->len;
	yystack_alloc(stack, stack->len + 1);
	/* stack->len increases above */
	stack->val[index] = val;
}

int
yystack_popn(YYStack *stack, int n)
{
	assert(0 < n && n < stack->len);
	for (int i = 0; i < n; i++) {
		stack->len--;
	}
	return stack->val[stack->len]; /* i.e. the last item popped */
}

int
yystack_top(YYStack *stack)
{
	assert(stack->len > 0);
	return stack->val[stack->len - 1];
}

int
yystack_1n(YYStack *stack, int n)
{
	assert(0 < n && n <= stack->len);
	return stack->val[stack->len - n];
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
yystaction(int state, int token, YYStack *values)
{
	int val;
	switch (state) {
	case 0:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case START:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case UNION:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case LCURL:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case TOKEN:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case LEFT:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case RIGHT:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case NONASSOC:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case TYPE:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 1:
		switch (token) {
		default:
			if (token <= 0) {
				val = yystack_1n(values, 1); 
				return yyaction_accept();
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 2:
		switch (token) {
		case MARK:
			return yyaction_shift(3);
		case START:
			return yyaction_shift(5);
		case UNION:
			return yyaction_shift(6);
		case LCURL:
			return yyaction_shift(7);
		case TOKEN:
			return yyaction_shift(9);
		case LEFT:
			return yyaction_shift(10);
		case RIGHT:
			return yyaction_shift(11);
		case NONASSOC:
			return yyaction_shift(12);
		case TYPE:
			return yyaction_shift(13);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 3:
		switch (token) {
		case C_IDENTIFIER:
			return yyaction_shift(15);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 4:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case START:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case UNION:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case LCURL:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case TOKEN:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case LEFT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case RIGHT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case NONASSOC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		case TYPE:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("defs", 2); /* defs -> defs def */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 5:
		switch (token) {
		case IDENTIFIER:
			return yyaction_shift(16);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 6:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("def", 1); /* def -> UNION */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 7:
		switch (token) {
		case RCURL:
			return yyaction_shift(17);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 8:
		switch (token) {
		case '<':
			return yyaction_shift(19);
		case IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 9:
		switch (token) {
		case '<':
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TOKEN */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 10:
		switch (token) {
		case '<':
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> LEFT */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 11:
		switch (token) {
		case '<':
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> RIGHT */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 12:
		switch (token) {
		case '<':
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> NONASSOC */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 13:
		switch (token) {
		case '<':
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("rword", 1); /* rword -> TYPE */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 14:
		switch (token) {
		case MARK:
			return yyaction_shift(22);
		case C_IDENTIFIER:
			return yyaction_shift(23);
		case '|':
			return yyaction_shift(24);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 0); 
				yystack_popn(values, 0);
				yystack_push(values, val);
				return yyaction_reduce("tail", 0); /* tail ->  */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 15:
		switch (token) {
		case PREC:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case ';':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case MARK:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case C_IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '|':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '{':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 0); 
				yystack_popn(values, 0);
				yystack_push(values, val);
				return yyaction_reduce("tail", 0); /* tail ->  */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 16:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case START:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case UNION:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case LCURL:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case TOKEN:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case LEFT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case RIGHT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case NONASSOC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		case TYPE:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> START IDENTIFIER */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 17:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case START:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case UNION:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case LCURL:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case TOKEN:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case LEFT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case RIGHT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case NONASSOC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		case TYPE:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("def", 2); /* def -> LCURL RCURL */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 18:
		switch (token) {
		case IDENTIFIER:
			return yyaction_shift(28);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 19:
		switch (token) {
		case IDENTIFIER:
			return yyaction_shift(29);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 20:
		switch (token) {
		default:
			if (token <= 0) {
				val = yystack_1n(values, 4); 
				yystack_popn(values, 4);
				yystack_push(values, val);
				return yyaction_reduce("spec", 4); /* spec -> defs MARK rules tail */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 21:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rules", 2); /* rules -> rules rule */
		case C_IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rules", 2); /* rules -> rules rule */
		case '|':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rules", 2); /* rules -> rules rule */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 2); 
				yystack_popn(values, 2);
				yystack_push(values, val);
				return yyaction_reduce("rules", 2); /* rules -> rules rule */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 22:
		switch (token) {
		default:
			if (token <= 0) {
				val = yystack_1n(values, 1); 
				yystack_popn(values, 1);
				yystack_push(values, val);
				return yyaction_reduce("tail", 1); /* tail -> MARK */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 23:
		switch (token) {
		case PREC:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case ';':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case MARK:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case C_IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '|':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '{':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 0); 
				yystack_popn(values, 0);
				yystack_push(values, val);
				return yyaction_reduce("tail", 0); /* tail ->  */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 24:
		switch (token) {
		case PREC:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case ';':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case MARK:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case C_IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '|':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '{':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 0); 
				yystack_popn(values, 0);
				yystack_push(values, val);
				return yyaction_reduce("tail", 0); /* tail ->  */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 25:
		switch (token) {
		case IDENTIFIER:
			return yyaction_shift(33);
		case PREC:
			return yyaction_shift(35);
		case MARK:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case C_IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '|':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case ';':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '{':
			return yyaction_shift(36);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 0); 
				yystack_popn(values, 0);
				yystack_push(values, val);
				return yyaction_reduce("tail", 0); /* tail ->  */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 26:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case START:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case UNION:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case LCURL:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case TOKEN:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case LEFT:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case RIGHT:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case NONASSOC:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case TYPE:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("def", 3); /* def -> rword tag nlist */
		case IDENTIFIER:
			return yyaction_shift(28);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 27:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		case IDENTIFIER:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 1); /* nlist -> nmno */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 28:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case START:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case UNION:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case LCURL:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case TOKEN:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case LEFT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case RIGHT:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case NONASSOC:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case TYPE:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case IDENTIFIER:
			val = yystack_1n(values, 1); 
			yystack_popn(values, 1);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 1); /* nmno -> IDENTIFIER */
		case NUMBER:
			return yyaction_shift(38);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 29:
		switch (token) {
		case '>':
			return yyaction_shift(39);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 30:
		switch (token) {
		case IDENTIFIER:
			return yyaction_shift(33);
		case PREC:
			return yyaction_shift(35);
		case MARK:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case C_IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '|':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case ';':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '{':
			return yyaction_shift(36);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 0); 
				yystack_popn(values, 0);
				yystack_push(values, val);
				return yyaction_reduce("tail", 0); /* tail ->  */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 31:
		switch (token) {
		case IDENTIFIER:
			return yyaction_shift(33);
		case PREC:
			return yyaction_shift(35);
		case MARK:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case C_IDENTIFIER:
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '|':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case ';':
			val = yystack_1n(values, 0); 
			yystack_popn(values, 0);
			yystack_push(values, val);
			return yyaction_reduce("tail", 0); /* tail ->  */
		case '{':
			return yyaction_shift(36);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 0); 
				yystack_popn(values, 0);
				yystack_push(values, val);
				return yyaction_reduce("tail", 0); /* tail ->  */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 32:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
		case C_IDENTIFIER:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
		case '|':
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
		case ';':
			return yyaction_shift(42);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 3); 
				yystack_popn(values, 3);
				yystack_push(values, val);
				return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 33:
		switch (token) {
		case PREC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
		case ';':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
		case C_IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
		case '|':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
		case IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
		case '{':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 2); 
				yystack_popn(values, 2);
				yystack_push(values, val);
				return yyaction_reduce("rbody", 2); /* rbody -> rbody IDENTIFIER */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 34:
		switch (token) {
		case PREC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
		case ';':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
		case C_IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
		case '|':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
		case IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
		case '{':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 2); 
				yystack_popn(values, 2);
				yystack_push(values, val);
				return yyaction_reduce("rbody", 2); /* rbody -> rbody act */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 35:
		switch (token) {
		case IDENTIFIER:
			return yyaction_shift(43);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 36:
		switch (token) {
		case '}':
			return yyaction_shift(44);
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 37:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case START:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case UNION:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case LCURL:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case TOKEN:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case LEFT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case RIGHT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case NONASSOC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case TYPE:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		case IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nlist", 2); /* nlist -> nlist nmno */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 38:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case START:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case UNION:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case LCURL:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case TOKEN:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case LEFT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case RIGHT:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case NONASSOC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case TYPE:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		case IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("nmno", 2); /* nmno -> IDENTIFIER NUMBER */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 39:
		switch (token) {
		case IDENTIFIER:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("tag", 3); /* tag -> < IDENTIFIER > */
		default:
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 40:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
		case C_IDENTIFIER:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
		case '|':
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
		case ';':
			return yyaction_shift(42);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 3); 
				yystack_popn(values, 3);
				yystack_push(values, val);
				return yyaction_reduce("rules", 3); /* rules -> C_IDENTIFIER rbody prec */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 41:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rule", 3); /* rule -> | rbody prec */
		case C_IDENTIFIER:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rule", 3); /* rule -> | rbody prec */
		case '|':
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("rule", 3); /* rule -> | rbody prec */
		case ';':
			return yyaction_shift(42);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 3); 
				yystack_popn(values, 3);
				yystack_push(values, val);
				return yyaction_reduce("rule", 3); /* rule -> | rbody prec */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 42:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> prec ; */
		case C_IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> prec ; */
		case '|':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> prec ; */
		case ';':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> prec ; */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 2); 
				yystack_popn(values, 2);
				yystack_push(values, val);
				return yyaction_reduce("prec", 2); /* prec -> prec ; */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 43:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> PREC IDENTIFIER */
		case C_IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> PREC IDENTIFIER */
		case '|':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> PREC IDENTIFIER */
		case ';':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("prec", 2); /* prec -> PREC IDENTIFIER */
		case '{':
			return yyaction_shift(36);
		default:
			if (token <= 0) {
				val = yystack_1n(values, 2); 
				yystack_popn(values, 2);
				yystack_push(values, val);
				return yyaction_reduce("prec", 2); /* prec -> PREC IDENTIFIER */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 44:
		switch (token) {
		case PREC:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("act", 2); /* act -> { } */
		case ';':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("act", 2); /* act -> { } */
		case MARK:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("act", 2); /* act -> { } */
		case C_IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("act", 2); /* act -> { } */
		case '|':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("act", 2); /* act -> { } */
		case IDENTIFIER:
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("act", 2); /* act -> { } */
		case '{':
			val = yystack_1n(values, 2); 
			yystack_popn(values, 2);
			yystack_push(values, val);
			return yyaction_reduce("act", 2); /* act -> { } */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 2); 
				yystack_popn(values, 2);
				yystack_push(values, val);
				return yyaction_reduce("act", 2); /* act -> { } */
			}
			fprintf(stderr, "invalid token %d in state %d\n", token, state);
			exit(EXIT_FAILURE);
		}
	case 45:
		switch (token) {
		case MARK:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("prec", 3); /* prec -> PREC IDENTIFIER act */
		case C_IDENTIFIER:
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("prec", 3); /* prec -> PREC IDENTIFIER act */
		case '|':
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("prec", 3); /* prec -> PREC IDENTIFIER act */
		case ';':
			val = yystack_1n(values, 3); 
			yystack_popn(values, 3);
			yystack_push(values, val);
			return yyaction_reduce("prec", 3); /* prec -> PREC IDENTIFIER act */
		default:
			if (token <= 0) {
				val = yystack_1n(values, 3); 
				yystack_popn(values, 3);
				yystack_push(values, val);
				return yyaction_reduce("prec", 3); /* prec -> PREC IDENTIFIER act */
			}
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
		if (strcmp(nt, "spec") == 0) {
			return 1;
		} else if (strcmp(nt, "defs") == 0) {
			return 2;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 1:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 2:
		if (strcmp(nt, "def") == 0) {
			return 4;
		} else if (strcmp(nt, "rword") == 0) {
			return 8;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 3:
		if (strcmp(nt, "rules") == 0) {
			return 14;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 4:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 5:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 6:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 7:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 8:
		if (strcmp(nt, "tag") == 0) {
			return 18;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 9:
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
	case 14:
		if (strcmp(nt, "tail") == 0) {
			return 20;
		} else if (strcmp(nt, "rule") == 0) {
			return 21;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 15:
		if (strcmp(nt, "rbody") == 0) {
			return 25;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 16:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 17:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 18:
		if (strcmp(nt, "nlist") == 0) {
			return 26;
		} else if (strcmp(nt, "nmno") == 0) {
			return 27;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 19:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 20:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 21:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 22:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 23:
		if (strcmp(nt, "rbody") == 0) {
			return 30;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 24:
		if (strcmp(nt, "rbody") == 0) {
			return 31;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 25:
		if (strcmp(nt, "prec") == 0) {
			return 32;
		} else if (strcmp(nt, "act") == 0) {
			return 34;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 26:
		if (strcmp(nt, "nmno") == 0) {
			return 37;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 27:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 28:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 29:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 30:
		if (strcmp(nt, "prec") == 0) {
			return 40;
		} else if (strcmp(nt, "act") == 0) {
			return 34;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 31:
		if (strcmp(nt, "prec") == 0) {
			return 41;
		} else if (strcmp(nt, "act") == 0) {
			return 34;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 32:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 33:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 34:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 35:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 36:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 37:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 38:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 39:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 40:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 41:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 42:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 43:
		if (strcmp(nt, "act") == 0) {
			return 45;
		}
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 44:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	case 45:
		fprintf(stderr, "invalid reduction '%s' in state %d\n", nt, state);
		exit(EXIT_FAILURE);
	}
	assert(false); /* unknown state */
}

int
yyparse()
{
	int token = yylex();
	YYStack *values = yystack_create(yylval);
	YYStack *states = yystack_create(0);
	while (true) {
		YYAction act = yystaction(yystack_top(states), token, values);
		switch (act.type) {
		case YYACTION_SHIFT:
			yystack_push(states, act.u.state);
			token = yylex();
			yystack_push(values, yylval);
			continue;
		case YYACTION_REDUCE:
			yystack_popn(states, act.u.r.len);
			yystack_push(states, yystgoto(yystack_top(states), act.u.r.nt));
			continue;
		case YYACTION_ACCEPT:
			yystack_destroy(states);
			yystack_destroy(values);
			return 0;
		}
		assert(false); /* invalid action type */
	}
}
