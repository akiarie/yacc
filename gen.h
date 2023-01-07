#ifndef YACC_GEN
#define YACC_GEN

#define YY_STATE_ACTION "yystaction"
#define YY_STATE_GOTO "yystgoto"
#define YY_REDUCE_VAL "val"

void
gen_headers(FILE *, Parser);

void
gen(FILE *, Parser);

#endif
