#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int
yylex();

struct parseresult {
	char *nt;	/* production head */
	size_t nret;	/* remaining returns */
};

struct symbol {
	union symbolval {
		int token;
		char *nt;
	} u;
	bool terminal;
};

struct symbol
symbol_yylex()
{
	return (struct symbol) {
		.u = (union symbolval) { .token = yylex() },
		.terminal = true,
	};
}

struct symbol
symbol_nt(char *nt)
{
	return (struct symbol) {
		.u = (union symbolval) { .nt = nt },
		.terminal = false,
	};
}

struct parseresult
yystate0(struct symbol),
	yystate1(struct symbol),
	yystate2(struct symbol),
	yystate3(struct symbol),
	yystate4(struct symbol),
	yystate5(struct symbol),
	yystate6(struct symbol),
	yystate7(struct symbol),
	yystate8(struct symbol),
	yystate9(struct symbol),
	yystate10(struct symbol),
	yystate11(struct symbol),
	yystate12(struct symbol),
	yystate13(struct symbol);

struct parseresult
yystate0(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(symbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "line") == 0) {
			r = yystate1(symbol_yylex());
		} else if (strcmp(nt, "expr") == 0) {
			r = yystate2(symbol_yylex());
		} else if (strcmp(nt, "term") == 0) {
			r = yystate3(symbol_yylex());
		} else if (strcmp(nt, "factor") == 0) {
			r = yystate4(symbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate0(symbol_nt(r.nt));
}

struct parseresult
yystate1(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		default:
			if (token <= 0) { /* EOF */
				/* acc */
				/* reduce line' -> line */
				r = (struct parseresult) { .nt = "line'", .nret = 1 };
			}
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate1(symbol_nt(r.nt));
}

struct parseresult
yystate2(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			r = yystate7(symbol_yylex());
			break;
		case 258: /* + */
			r = yystate8(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate2(symbol_nt(r.nt));
}

struct parseresult
yystate3(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce expr -> term */
			r = (struct parseresult) { .nt = "expr", .nret = 1 };
			break;
		case 258: /* + */
			/* reduce expr -> term */
			r = (struct parseresult) { .nt = "expr", .nret = 1 };
			break;
		case 261: /* ) */
			/* reduce expr -> term */
			r = (struct parseresult) { .nt = "expr", .nret = 1 };
			break;
		case 259: /* * */
			r = yystate9(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate3(symbol_nt(r.nt));
}

struct parseresult
yystate4(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce term -> factor */
			r = (struct parseresult) { .nt = "term", .nret = 1 };
			break;
		case 258: /* + */
			/* reduce term -> factor */
			r = (struct parseresult) { .nt = "term", .nret = 1 };
			break;
		case 261: /* ) */
			/* reduce term -> factor */
			r = (struct parseresult) { .nt = "term", .nret = 1 };
			break;
		case 259: /* * */
			/* reduce term -> factor */
			r = (struct parseresult) { .nt = "term", .nret = 1 };
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate4(symbol_nt(r.nt));
}

struct parseresult
yystate5(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(symbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "expr") == 0) {
			r = yystate10(symbol_yylex());
		} else if (strcmp(nt, "term") == 0) {
			r = yystate3(symbol_yylex());
		} else if (strcmp(nt, "factor") == 0) {
			r = yystate4(symbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate5(symbol_nt(r.nt));
}

struct parseresult
yystate6(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce factor -> id */
			r = (struct parseresult) { .nt = "factor", .nret = 1 };
			break;
		case 258: /* + */
			/* reduce factor -> id */
			r = (struct parseresult) { .nt = "factor", .nret = 1 };
			break;
		case 261: /* ) */
			/* reduce factor -> id */
			r = (struct parseresult) { .nt = "factor", .nret = 1 };
			break;
		case 259: /* * */
			/* reduce factor -> id */
			r = (struct parseresult) { .nt = "factor", .nret = 1 };
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate6(symbol_nt(r.nt));
}

struct parseresult
yystate7(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		default:
			if (token <= 0) { /* EOF */
				/* acc */
				/* reduce line' -> line */
				r = (struct parseresult) { .nt = "line'", .nret = 1 };
			}
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate7(symbol_nt(r.nt));
}

struct parseresult
yystate8(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(symbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "term") == 0) {
			r = yystate11(symbol_yylex());
		} else if (strcmp(nt, "factor") == 0) {
			r = yystate4(symbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate8(symbol_nt(r.nt));
}

struct parseresult
yystate9(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(symbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "factor") == 0) {
			r = yystate12(symbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate9(symbol_nt(r.nt));
}

struct parseresult
yystate10(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 261: /* ) */
			r = yystate13(symbol_yylex());
			break;
		case 258: /* + */
			r = yystate8(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate10(symbol_nt(r.nt));
}

struct parseresult
yystate11(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce expr -> expr + term */
			r = (struct parseresult) { .nt = "expr", .nret = 3 };
			break;
		case 258: /* + */
			/* reduce expr -> expr + term */
			r = (struct parseresult) { .nt = "expr", .nret = 3 };
			break;
		case 261: /* ) */
			/* reduce expr -> expr + term */
			r = (struct parseresult) { .nt = "expr", .nret = 3 };
			break;
		case 259: /* * */
			r = yystate9(symbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate11(symbol_nt(r.nt));
}

struct parseresult
yystate12(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce term -> term * factor */
			r = (struct parseresult) { .nt = "term", .nret = 3 };
			break;
		case 258: /* + */
			/* reduce term -> term * factor */
			r = (struct parseresult) { .nt = "term", .nret = 3 };
			break;
		case 261: /* ) */
			/* reduce term -> term * factor */
			r = (struct parseresult) { .nt = "term", .nret = 3 };
			break;
		case 259: /* * */
			/* reduce term -> term * factor */
			r = (struct parseresult) { .nt = "term", .nret = 3 };
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate12(symbol_nt(r.nt));
}

struct parseresult
yystate13(struct symbol s)
{
	struct parseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce factor -> ( expr ) */
			r = (struct parseresult) { .nt = "factor", .nret = 3 };
			break;
		case 258: /* + */
			/* reduce factor -> ( expr ) */
			r = (struct parseresult) { .nt = "factor", .nret = 3 };
			break;
		case 261: /* ) */
			/* reduce factor -> ( expr ) */
			r = (struct parseresult) { .nt = "factor", .nret = 3 };
			break;
		case 259: /* * */
			/* reduce factor -> ( expr ) */
			r = (struct parseresult) { .nt = "factor", .nret = 3 };
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct parseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate13(symbol_nt(r.nt));
}

int
yyparse()
{
	struct parseresult r = yystate0(symbol_yylex());
	if (r.nret != 0 || strcmp(r.nt, "line'") != 0) {
		fprintf(stderr, "unable to parse");
		return 1;
	}
	return 0;
}
