#ifndef BNF_LEX
#define BNF_LEX

enum tktype {
	TK_EOF		= 0,		/* "...the endmarker must have token
					   number 0 or negative." */
	/* basic entries */
	TK_IDENTIFIER	= 1 << 0,	/* includes identifiers and literals */
	TK_C_IDENTIFIER	= 1 << 1,	/* identifier (but not literal) followed
					   by a : */
	TK_NUMBER	= 1 << 2,	/* [0-9][0-9]* */

	/* reserved words : %type=>TYPE %left=>LEFT, and so on */
	TK_LEFT		= 1 << 3,
	TK_RIGHT	= 1 << 4,
	TK_NONASSOC	= 1 << 5,
	TK_TOKEN	= 1 << 6,
	TK_PREC		= 1 << 7,
	TK_TYPE		= 1 << 8,
	TK_START	= 1 << 9,
	TK_UNION	= 1 << 10,
	TK_MARK		= 1 << 11,	/* the %% mark */
	TK_LCURL	= 1 << 12,      /* the %{ mark */
	TK_RCURL	= 1 << 13,      /* the %} mark */

	/* internals */
	TK_ERROR	= 1 << 14,
};

#endif
