#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "table.h"
#include "util.h"

Cell
cell_string(char *s)
{
	return (Cell) { .string = true, .u = { .s = s } };
}

Cell
cell_table(struct table *T)
{
	return (Cell) { .string = false, .u = { .T = T } };
}

static Cell
cell_parse(void *arg, bool string)
{
	if (string) {
		return cell_string(arg ? (char *) arg : "");
	} else {
		return cell_table((struct table *) arg);
	}
}

Row *
row_create(char *order)
{
	Row *r = calloc(1, sizeof(Row));
	r->order = order;
	r->cap = strlen(order);
	return r;
}

static void
row_appendcell(Row *r, Cell cell)
{
	assert(r->len < r->cap);
	assert(r->order[r->len] == (cell.string ? 's' : 't'));
	r->cell = realloc(r->cell, sizeof(Cell) * ++r->len);
	r->cell[r->len - 1] = cell;
}

void
row_appendstring(Row *r, char *s)
{
	row_appendcell(r, cell_string(s));
}

void
row_appendtable(Row *r, struct table *T)
{
	row_appendcell(r, cell_table(T));
}

Row *
row_inline_act(char *order, ...)
{
	Row *r = row_create(order);
	va_list ap;
	va_start(ap, order);
	for (int i = 0; i < r->cap; i++) {
		row_appendcell(r, cell_parse(va_arg(ap, void *),
			order[i] == 's'));
	}
	va_end(ap);
	return r;
}

Table
table_create(size_t ncol)
{
	return (Table) {
		.row	= NULL,
		.ncol	= ncol,
		.nrow	= 0,
	};
}

void
table_destroy(Table *T)
{
	for (int i = 0; i < T->nrow; i++) {
		free(T->row[i]);
	}
	free(T->row);
}

void
table_append_act(Table *T, Row *r0, ...)
{
	va_list ap;
	va_start(ap, r0);
	for (Row *r = r0; r; r = va_arg(ap, Row *)) {
		T->row = realloc(T->row, sizeof(Cell *) * ++T->nrow);
		T->row[T->nrow - 1] = r->cell;
	}
	va_end(ap);
}

static size_t
max(size_t a, size_t b)
{
	return a > b ? a : b;
}

static size_t *
colspacings(Table *T)
{
	size_t *col = calloc(T->ncol, sizeof(size_t));
	for (int i = 0; i < T->nrow; i++) {
		for (int j = 0; j < T->ncol; j++) {
			col[j] = max(col[j], strlen(T->row[i][j].u.s));
		}
	}
	return col;
}

struct printsettings {
	size_t *colwidth; /* widths of columns printed */
	enum table_printmode mode;

	char *colsep, *rowsep, *corner;
	char *align;
};

static char *
sprint_colsep(Table *T, int col, struct printsettings st)
{
	struct strbuilder *b = strbuilder_create();
	if (col + 1 < T->ncol) {
		strbuilder_printf(b, "%s", st.colsep);
	}
	return strbuilder_build(b);
}

static char *
sprint_cell(char *row, size_t width, char align)
{
	struct strbuilder *b = strbuilder_create();
	switch (align) {
	case 'l':
		strbuilder_printf(b, "%-*s", width, row);
		break;
	case 'r':
		strbuilder_printf(b, "%*s", width, row);
		break;
	case 'c': {
		int pad = (width - strlen(row)); assert(pad >= 0);
		int lpad = pad / 2;
		int rpad = pad - lpad;
		strbuilder_printf(b, "%*s%s%*s", lpad, "", row, rpad, "");
		break;
	}
	default:
		assert(false);
	}
	return strbuilder_build(b);
}

static int
strbuilder_printf_cond(struct strbuilder *b, bool cond, const char *fmt, ...)
{
	if (!cond) {
		return 0;
	}
	va_list ap;
	va_start(ap, fmt);
	int r = strbuilder_vprintf(b, fmt, ap);
	va_end(ap);
	return r;
}

#define COL_SPACE " "

static char *
sprint_rowdiv(Table *T, struct printsettings st)
{
	bool coldiv = st.mode & PM_COLDIV;
	struct strbuilder *b = strbuilder_create();
	/* left corner */
	strbuilder_printf_cond(b, st.mode & PM_EXTERIOR, "%s", st.corner);
	for (int i = 0; i < T->ncol; i++) {
		int colspace = coldiv ? strlen(COL_SPACE) * 2 : 0;
		size_t truewidth = st.colwidth[i] + colspace;
		for (int j = 0; j < truewidth; j++) {
			strbuilder_printf(b, "%s", st.rowsep);
		}
		/* right col corner */
		if (coldiv && (i + 1 < T->ncol)) {
			strbuilder_printf(b, st.corner);
		}
	}
	/* final corner */
	strbuilder_printf_cond(b, st.mode & PM_EXTERIOR, st.corner);
	strbuilder_putc(b, '\n');
	return strbuilder_build(b);
}

static char *
sprint_columned(Table *T, int row, int col, struct printsettings st)
{
	struct strbuilder *b = strbuilder_create();
	char *colsep = sprint_colsep(T, col, st);
	strbuilder_printf_cond(b, st.mode & PM_COLDIV, COL_SPACE);
	char *e = sprint_cell(T->row[row][col].u.s, st.colwidth[col], st.align[col]);
	strbuilder_printf(b, e);
	free(e);
	strbuilder_printf_cond(b, st.mode & PM_COLDIV, "%s%s", COL_SPACE, colsep);
	free(colsep);
	return strbuilder_build(b);
}

static char *
sprint_rowed(Table *T, int row, struct printsettings st)
{
	struct strbuilder *b = strbuilder_create();
	/* left line */
	strbuilder_printf_cond(b, st.mode & PM_EXTERIOR, "%s", st.colsep);
	for (int j = 0; j < T->ncol; j++) {
		char *column = sprint_columned(T, row, j, st);
		strbuilder_printf(b, "%s", column);
		free(column);
	}
	/* right line */
	strbuilder_printf_cond(b, st.mode & PM_EXTERIOR, "%s", st.colsep);
	return strbuilder_build(b);
}

static char *
table_str_act(Table *T, struct printsettings st)
{
	struct strbuilder *b = strbuilder_create();
	char *rowdiv = sprint_rowdiv(T, st);
	/* top line */
	strbuilder_printf_cond(b, st.mode & PM_EXTERIOR, "%s", rowdiv);
	for (int i = 0; i < T->nrow; i++) {
		char *row = sprint_rowed(T, i, st);
		strbuilder_printf(b, "%s\n", row);
		free(row);
		/* interior-row line */
		if ((st.mode & PM_ROWDIV) && (i + 1 < T->nrow)) {
			strbuilder_printf(b, "%s", rowdiv);
		}
	}
	/* final line */
	strbuilder_printf_cond(b, st.mode & PM_EXTERIOR, "%s", rowdiv);
	free(rowdiv);
	free(st.colwidth);
	return strbuilder_build(b);
}

static bool
validalign(size_t ncol, char *align)
{
	int len = strlen(align);
	for (int i = 0; i < len; i++) {
		if (strchr("lcr", align[i]) == NULL) {
			return false;
		}
	}
	return ncol == len;
}

char *
table_str(Table *T, enum table_printmode mode, char *align)
{
	assert(validalign(T->ncol, align));
	return table_str_act(T, (struct printsettings) {
		.colwidth	= colspacings(T),
		.mode		= mode,
		.colsep		= DEFAULT_COLSEP,
		.rowsep		= DEFAULT_ROWSEP,
		.corner		= DEFAULT_CORNER,
		.align		= align,
	});
}
