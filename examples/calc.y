%{
#include <stdio.h>
#include <ctype.h>

int
yylex();
%}

%token DIGIT

%%
line	: expr '\n'		{ printf("%d\n", $1); }
     	;
expr	: expr '+' term		{ $$ = $1 + $3; }
	| term
	;
term	: term '*' factor	{ $$ = $1 * $3; }
     	| factor
	;
factor	: '(' expr ')'		{ $$ = $2; }
	| DIGIT
	;
%%
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
