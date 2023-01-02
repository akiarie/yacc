#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "grammar.h"

typedef Grammar * (*filter)(const Grammar *);

bool
filter_apply(filter f, char *input, char *expected)
{
	Grammar *S = grammar_parse(input);
	Grammar *SS = f(S);
	Grammar *exp = grammar_parse(expected);
	bool succ = grammar_eq(SS, exp);
	if (!succ) {
		gprintf("input:\n%j\n\n"
			"got:\n%j\n\n"
			"expected:\n%j\n", S, SS, exp);
	}
	grammar_destroy(S);
	grammar_destroy(SS);
	grammar_destroy(exp);
	return succ;
}

bool
sa()
{
	char *S =
	"S : A a | b;"
	"A : A c | S d | <e>;";
	char *exp =
	"S  : A a | b;"
	"A  : b d A' | A';"
	"A' : c A' | a d A' | <e>;";
	return filter_apply(grammar_unleftrec, S, exp);
}

bool
expr()
{
	char *S =
	 "E : E + T | T;"
	 "T : T * F | F;"
	 "F : ( E ) | id;";
	char *exp =
	 "E  : T E';"
	 "E' : + T E' | <e>;"
	 "T  : F T';"
	 "T' : * F T' | <e>;"
	 "F  : ( E ) | id;";
	return filter_apply(grammar_unleftrec, S, exp);
}

bool
prefixabc()
{
	char *S = "S : a | a b | a b c | d | d e;";
	char *exp =
	"S    : a S' | d S'';"
	"S'   : b S''' | <e>;"
	"S''  : e | <e>;"
	"S''' : c | <e>;";
	return filter_apply(grammar_leftfactor, S, exp);
}

#define LEN(a) (sizeof(a) / sizeof((a)[0]))

typedef Symbolset * (symbolfunc)(const Grammar *, char *);

bool testsymbolfunc(symbolfunc sf, Grammar *G, char *sym, Symbolset *exp)
{
	Symbolset *s = sf(G, sym);
	bool succ = symbolset_eq(exp, s);
	if (!succ) {
		fprintf(stderr, "expected {%s} got {%s}\n", prod_str(exp, G),
			prod_str(s, G));
	}
	return succ;
}

bool
dangle()
{
	char *S =
	 "S : i E t S | i E t S e S | a;"
	 "E : b;";
	char *exp =
	"S  : i E t S S' | a;"
	"S' : e S | <e>;"
	"E  : b;";
	if (!filter_apply(grammar_leftfactor, S, exp)) {
		return false;
	}
	Grammar *G = grammar_parse(exp);
	struct testresult { char *sym; Symbolset *exp; };
	/* FIRST */
	struct testresult t1[] = {
		{"S",  symbolset_create("i", "a")},
		{"S'", symbolset_create("e", "<e>")},
		{"E",  symbolset_create("b")},
	};
	int len = LEN(t1);
	for (int i = 0; i < len; i++) {
		if (!testsymbolfunc(grammar_first, G, t1[i].sym, t1[i].exp)) {
			fprintf(stderr, "t1 failed on %s\n", t1[i].sym);
			return false;
		}
	}
	/* FOLLOW */
	struct testresult t2[] = {
		{"S",  symbolset_create("$", "e")},
		{"S'", symbolset_create("$", "e")},
		{"E",  symbolset_create("t")},
	};
	len = LEN(t2);
	for (int i = 0; i < len; i++) {
		if (!testsymbolfunc(grammar_follow, G, t2[i].sym, t2[i].exp)) {
			fprintf(stderr, "t2 failed on %s\n", t2[i].sym);
			return false;
		}
	}
	return true;
}

bool
ex431()
{
	char *S =
	 "rexpr    : rexpr + rterm | rterm;"
	 "rterm    : rterm rfactor | rfactor;"
	 "rfactor  : rfactor * | rprimary;"
	 "rprimary : a | b;";
	bool succ = filter_apply(grammar_leftfactor, S, S);
	char *exp =
	"rexpr    : rterm rexpr';"
	"rexpr'   : + rterm rexpr' | <e>;"
	"rterm    : rfactor rterm';"
	"rterm'   : rfactor rterm' | <e>;"
	"rfactor  : rprimary rfactor';"
	"rfactor' : * rfactor' | <e>;"
	"rprimary : a | b;";
	return succ && filter_apply(grammar_unleftrec, S, exp);
}

bool
ex432a()
{
	char *S = "S : S S + | S S * | a";
	char *exp =
	"S  : S S S' | a;"
	"S' : + | *;";
	bool succ = filter_apply(grammar_leftfactor, S, exp);
	S =
	"S    : S S sign | a;"
	"sign : + | *;";
	exp =
	"S    : a S';"
	"S'   : S sign S' | <e>;"
	"sign : + | *;";
	return succ && filter_apply(grammar_unleftrec, S, exp);
}

bool
ex432b()
{
	char *S = "S : 0 S 1 | 0 1;";
	char *exp =
	"S  : 0 S';"
	"S' : S 1 | 1;";
	return filter_apply(grammar_leftfactor, S, exp);
}

bool
ex432c()
{
	char *S = "S : S ( S ) S | <e>;";
	char *exp =
	"S  : S';"
	"S' : ( S ) S S' | <e>";
	return filter_apply(grammar_leftfactor, S, S) &&
		filter_apply(grammar_unleftrec, S, exp);
}

bool
ex432d()
{
	char *S =
	"L : L , S | S;" /* inverted ordering b/c of overstepping */
	"S : ( L ) | a;";
	char *exp =
	"S  : ( L ) | a;"
	"L  : S L';"
	"L' : , S L' | <e>";
	return filter_apply(grammar_leftfactor, S, S) &&
		filter_apply(grammar_unleftrec, S, exp);
}

bool
ex432e()
{
	char *S =
	"bexpr   : bexpr or bterm | bterm;"
	"bterm   : bterm and bfactor | bfactor;"
	"bfactor : not bfactor | ( bexpr ) | true | false;";
	char *exp =
	"bexpr   : bterm bexpr';"
	"bexpr'  : or bterm bexpr' | <e>;"
	"bterm   : bfactor bterm';"
	"bterm'  : and bfactor bterm' | <e>;"
	"bfactor : not bfactor | ( bexpr ) | true | false;";
	return filter_apply(grammar_leftfactor, S, S) &&
		filter_apply(grammar_unleftrec, S, exp);
}

typedef bool (*test)();

bool
ex432()
{
	struct { char *name; test test; } tests[] = {
		{ "a",	ex432a, },
		{ "b",	ex432b, },
		{ "c",	ex432c, },
		{ "d",	ex432d, },
		{ "e",	ex432e, },
	};
	for (int i = 0, len = LEN(tests); i < len; i++) {
		if (!tests[i].test()) {
			fprintf(stderr, "%s failed\n", tests[i].name);
			return false;
		}
	}
	return true;
}

bool
eg430()
{
	Grammar *G = grammar_parse(
		"E  : T E';"
		"E' : + T E' | <e>;"
		"T  : F T';"
		"T' : * F T' | <e>;"
		"F  : ( E ) | id;"
	);
	struct testresult { char *sym; Symbolset *exp; };
	/* FIRST */
	struct testresult t1[] = {
		{"F",  symbolset_create("(", "id")},
		{"T",  symbolset_create("(", "id")},
		{"E",  symbolset_create("(", "id")},
		{"E'", symbolset_create("+", "<e>")},
		{"T'", symbolset_create("*", "<e>")},
	};
	int len = LEN(t1);
	for (int i = 0; i < len; i++) {

		if (!testsymbolfunc(grammar_first, G, t1[i].sym, t1[i].exp)) {
			fprintf(stderr, "t1 failed on %s\n", t1[i].sym);
			return false;
		}
	}
	/* FOLLOW */
	struct testresult t2[] = {
		{"E",  symbolset_create(")", "$")},
		{"E'", symbolset_create(")", "$")},
		{"T",  symbolset_create("+", ")", "$")},
		{"T'", symbolset_create("+", ")", "$")},
		{"F",  symbolset_create("*", "+", ")", "$")},
	};
	len = LEN(t2);
	for (int i = 0; i < len; i++) {
		if (!testsymbolfunc(grammar_follow, G, t2[i].sym, t2[i].exp)) {
			fprintf(stderr, "t2 failed on %s\n", t2[i].sym);
			return false;
		}
	}
	return true;
}

bool
ll1()
{
	Grammar *G = grammar_parse(
		"E  : E + T | T;"
		"T  : T * F | F;"
		"F  : ( E ) | id;"
	);
	return !grammar_isLL1(G) && grammar_isLL1(grammar_unleftrec(G));
}

int main()
{
	struct { char *name; test test; } tests[] = {
		{ "expr",		expr, },
		{ "indirect",		sa, },
		{ "prefixabc",		prefixabc, },
		{ "dangle",		dangle, },
		{ "exercise 4.3.1",	ex431, },
		{ "exercise 4.3.2",	ex432, },
		{ "example 4.3.0",	eg430, },
		{ "LL(1)",		ll1, },
	};
	for (int i = 0, len = LEN(tests); i < len; i++) {
		if (!tests[i].test()) {
			fprintf(stderr, "%s failed\n", tests[i].name);
			exit(EXIT_FAILURE);
		}
	}
}
