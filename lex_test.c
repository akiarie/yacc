#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "lex.h"

#define EXAMPLE_FILE "examples/calc.y"

/* read_file: reads contents of file and returns them
 * caller must free returned string
 * see https://stackoverflow.com/a/14002993 */
char *
read_file(char *path)
{
	FILE *f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  /* same as rewind(f); */
	char *str = malloc(fsize + 1);
	fread(str, fsize, 1, f);
	fclose(f);
	str[fsize] = '\0';
	return str;
}

int
main()
{
	char *file = read_file(EXAMPLE_FILE);
	yyscanstring(file);
	int tk;
	while ((tk = yylex()) != TK_EOF) {
		if (tk == TK_ERROR) {
			fprintf(stderr, "error\n");
			exit(EXIT_FAILURE);
		}
	}
	free(file);
}
