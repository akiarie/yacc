#ifndef YY_LEX
#define YY_LEX

#define TK_EOF 0
#define TK_ERROR -1

int yylval;

int
yylex();

void
yyscanstring(char *);

char *
yytokstr(int tk);

#endif
