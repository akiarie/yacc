#ifndef BNF_TABLE
#define BNF_TABLE

struct table;

typedef struct {
	bool string; /* false iff table */
	union {
		char *s;
		struct table *T;
	} u;
} Cell;

typedef struct {
	Cell *cell;
	char *order;
	size_t len, cap; /* cap == strlen(order), len is last appended column */
} Row;

Row *
row_create(char *order);

void
row_appendstring(Row *, char *s);

void
row_appendtable(Row *, struct table *T);

Row *
row_inline_act(char *order, ...);

/* row_inline: order is a string of the form '[st]+' which specifies how
 * the columns are to be interpreted, as s-trings or t-ables. order must be
 * length T->ncol */
#define row_inline(order, ...) \
	row_inline_act(order, __VA_ARGS__, NULL)

typedef struct table {
	Cell **row;
	size_t nrow, ncol;
	char *order; /* stored for sanity checks */
} Table;

Table
table_create(size_t ncol);

void
table_destroy(Table *);

void
table_append_act(Table *, Row *, ...);

#define table_append(T, ...) \
	table_append_act(T, __VA_ARGS__, NULL)

enum table_printmode {
	PM_COMPACT	= 0,
	PM_ROWDIV	= 1 << 0,
	PM_COLDIV	= 1 << 1,
	PM_EXTERIOR	= 1 << 2,
	PM_ENTIRE	= PM_ROWDIV | PM_COLDIV | PM_EXTERIOR,
};

#define DEFAULT_COLSEP "|"
#define DEFAULT_ROWSEP "—"
#define DEFAULT_CORNER "+"

/* table_str: align is string of the form '[lcr]+' indicating how a given column
 * in the table is to be aligned. must be length ncol */
char *
table_str(Table *, enum table_printmode, char *align);

#endif
