#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int
yylex();

struct yyparseresult {
	char *nt;	/* production head */
	size_t nret;	/* remaining returns */
};

struct yysymbol {
	union yysymbolval {
		int token;
		char *nt;
	} u;
	bool terminal;
};

struct yysymbol
yysymbol_yylex()
{
	return (struct yysymbol) {
		.u = (union yysymbolval) { .token = yylex() },
		.terminal = true,
	};
}

struct yysymbol
yysymbol_nt(char *nt)
{
	return (struct yysymbol) {
		.u = (union yysymbolval) { .nt = nt },
		.terminal = false,
	};
}

struct yyparseresult
yystate0(struct yysymbol),
	yystate1(struct yysymbol),
	yystate2(struct yysymbol),
	yystate3(struct yysymbol),
	yystate4(struct yysymbol),
	yystate5(struct yysymbol),
	yystate6(struct yysymbol),
	yystate7(struct yysymbol),
	yystate8(struct yysymbol),
	yystate9(struct yysymbol),
	yystate10(struct yysymbol),
	yystate11(struct yysymbol),
	yystate12(struct yysymbol),
	yystate13(struct yysymbol);

struct yyparseresult
yystate0(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(yysymbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(yysymbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "line") == 0) {
			r = yystate1(yysymbol_yylex());
		} else if (strcmp(nt, "expr") == 0) {
			r = yystate2(yysymbol_yylex());
		} else if (strcmp(nt, "term") == 0) {
			r = yystate3(yysymbol_yylex());
		} else if (strcmp(nt, "factor") == 0) {
			r = yystate4(yysymbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate0(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate1(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		default:
			if (token <= 0) { /* EOF */
				/* acc */
				/* reduce line' -> line */
				r = (struct yyparseresult) { .nt = "line'", .nret = 1 };
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate1(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate2(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			r = yystate7(yysymbol_yylex());
			break;
		case 258: /* + */
			r = yystate8(yysymbol_yylex());
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate2(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate3(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce expr -> term */
			r = (struct yyparseresult) { .nt = "expr", .nret = 1 };
			break;
		case 258: /* + */
			/* reduce expr -> term */
			r = (struct yyparseresult) { .nt = "expr", .nret = 1 };
			break;
		case 261: /* ) */
			/* reduce expr -> term */
			r = (struct yyparseresult) { .nt = "expr", .nret = 1 };
			break;
		case 259: /* * */
			r = yystate9(yysymbol_yylex());
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate3(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate4(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce term -> factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 1 };
			break;
		case 258: /* + */
			/* reduce term -> factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 1 };
			break;
		case 261: /* ) */
			/* reduce term -> factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 1 };
			break;
		case 259: /* * */
			/* reduce term -> factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 1 };
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate4(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate5(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(yysymbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(yysymbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "expr") == 0) {
			r = yystate10(yysymbol_yylex());
		} else if (strcmp(nt, "term") == 0) {
			r = yystate3(yysymbol_yylex());
		} else if (strcmp(nt, "factor") == 0) {
			r = yystate4(yysymbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate5(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate6(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce factor -> id */
			r = (struct yyparseresult) { .nt = "factor", .nret = 1 };
			break;
		case 258: /* + */
			/* reduce factor -> id */
			r = (struct yyparseresult) { .nt = "factor", .nret = 1 };
			break;
		case 261: /* ) */
			/* reduce factor -> id */
			r = (struct yyparseresult) { .nt = "factor", .nret = 1 };
			break;
		case 259: /* * */
			/* reduce factor -> id */
			r = (struct yyparseresult) { .nt = "factor", .nret = 1 };
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate6(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate7(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		default:
			if (token <= 0) { /* EOF */
				/* acc */
				/* reduce line' -> line */
				r = (struct yyparseresult) { .nt = "line'", .nret = 1 };
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate7(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate8(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(yysymbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(yysymbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "term") == 0) {
			r = yystate11(yysymbol_yylex());
		} else if (strcmp(nt, "factor") == 0) {
			r = yystate4(yysymbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate8(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate9(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 260: /* ( */
			r = yystate5(yysymbol_yylex());
			break;
		case 262: /* id */
			r = yystate6(yysymbol_yylex());
			break;
		default:
			fprintf(stderr, "invalid token '%d'", token);
			exit(EXIT_FAILURE);
		}
	} else {
		char *nt = s.u.nt;
		if (strcmp(nt, "factor") == 0) {
			r = yystate12(yysymbol_yylex());
		} else {
			fprintf(stderr, "invalid nonterminal '%s'", nt);
			exit(EXIT_FAILURE);
		}
	}
	/* pop stack until no more returns and then recurse on the state
	 * where we land */
	if (r.nret > 0) {
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate9(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate10(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 261: /* ) */
			r = yystate13(yysymbol_yylex());
			break;
		case 258: /* + */
			r = yystate8(yysymbol_yylex());
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate10(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate11(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce expr -> expr + term */
			r = (struct yyparseresult) { .nt = "expr", .nret = 3 };
			break;
		case 258: /* + */
			/* reduce expr -> expr + term */
			r = (struct yyparseresult) { .nt = "expr", .nret = 3 };
			break;
		case 261: /* ) */
			/* reduce expr -> expr + term */
			r = (struct yyparseresult) { .nt = "expr", .nret = 3 };
			break;
		case 259: /* * */
			r = yystate9(yysymbol_yylex());
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate11(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate12(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce term -> term * factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 3 };
			break;
		case 258: /* + */
			/* reduce term -> term * factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 3 };
			break;
		case 261: /* ) */
			/* reduce term -> term * factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 3 };
			break;
		case 259: /* * */
			/* reduce term -> term * factor */
			r = (struct yyparseresult) { .nt = "term", .nret = 3 };
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate12(yysymbol_nt(r.nt));
}

struct yyparseresult
yystate13(struct yysymbol s)
{
	struct yyparseresult r;
	if (s.terminal) {
		int token = s.u.token;
		switch (token) {
		case 257: /* \n */
			/* reduce factor -> ( expr ) */
			r = (struct yyparseresult) { .nt = "factor", .nret = 3 };
			break;
		case 258: /* + */
			/* reduce factor -> ( expr ) */
			r = (struct yyparseresult) { .nt = "factor", .nret = 3 };
			break;
		case 261: /* ) */
			/* reduce factor -> ( expr ) */
			r = (struct yyparseresult) { .nt = "factor", .nret = 3 };
			break;
		case 259: /* * */
			/* reduce factor -> ( expr ) */
			r = (struct yyparseresult) { .nt = "factor", .nret = 3 };
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
		return (struct yyparseresult) {
			.nt = r.nt, .nret = r.nret - 1,
		};
	}
	return yystate13(yysymbol_nt(r.nt));
}

int
yyparse()
{
	struct yyparseresult r = yystate0(yysymbol_yylex());
	if (r.nret != 0 || strcmp(r.nt, "line'") != 0) {
		fprintf(stderr, "unable to parse");
		return 1;
	}
	return 0;
}
