%{
#include <stdio.h> /* FIXME */
%}
/* grammar for the input to yacc
 * https://www.unix.com/man-page/POSIX/1posix/yacc */

/* basic entries */
/* the following are recognized by the lexical analyzer */
%token IDENTIFIER      /* includes identifiers and literals */
%token C_IDENTIFIER    /* identifier (but not literal) followed by a : */
%token NUMBER          /* [0-9][0-9]* */

/* reserved words : %type=>TYPE %left=>LEFT, and so on */
%token LEFT RIGHT NONASSOC TOKEN PREC TYPE START UNION
%token MARK            /* the %% mark */
%token LCURL           /* the %{ mark */
%token RCURL           /* the %} mark */

/* 8-bit character literals stand for themselves; */
/* tokens have to be defined for multi-byte characters */
%start spec
%%

spec  : defs MARK rules tail
      ;
tail  : MARK { /* in this action, set up the rest of the file */ }
      | /* empty: the second MARK is optional */
      ;
defs  : /* empty */
      | defs def
      ;
def   : START IDENTIFIER
      | UNION { /* copy union definition to output */ }
      | LCURL { /* copy C code to output file */ } RCURL
      | rword tag nlist
      ;
rword : TOKEN
      | LEFT
      | RIGHT
      | NONASSOC
      | TYPE
      ;
tag   : /* empty: union tag optional */
      | '<' IDENTIFIER '>'
      ;
nlist : nmno
      | nlist nmno
      ;
nmno  : IDENTIFIER         /* note: literal invalid with % type */
      | IDENTIFIER NUMBER  /* note: invalid with % type */
      ;

/* rules section */
rules : C_IDENTIFIER rbody prec
      | rules rule
      ;
rule  : C_IDENTIFIER rbody prec
      | '|' rbody prec
      ;
rbody : /* empty */
      | rbody IDENTIFIER
      | rbody act
      ;
act   : '{' { /* copy action, translate $$, and so on */ } '}'
      ;
prec  : /* empty */
      | PREC IDENTIFIER
      | PREC IDENTIFIER act
      | prec ';'
      ;
