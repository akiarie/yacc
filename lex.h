#ifndef YY_LEX
#define YY_LEX

#define TK_EOF 0
#define TK_ERROR -1

int
yylex();

void
yyerror(char *);

void
yyscanstring(char *);

char *
yytokstr(int tk);

int yylval;

char *
yylexeme();

char *
yytext();

#endif
