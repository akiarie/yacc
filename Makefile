CC = cc -g
HEADERS = grammar.h util.h table.h parser.h gen.h
OBJECTS = grammar.o util.o table.o parser.o gen.o

GRAMMAR_INC = grammar_parse.c grammar_util.c grammar_lr.c

grammar.o: grammar.h grammar.c $(GRAMMAR_INC)
	@printf '\CC\t$@\n'
	@$(CC) -c grammar.c

util.o: util.h util.c
	@printf '\CC\t$@\n'
	@$(CC) -c util.c

table.o: table.h table.c
	@printf '\CC\t$@\n'
	@$(CC) -c table.c

gen.o: gen.h gen.c
	@printf '\CC\t$@\n'
	@$(CC) -c gen.c

PARSER_SRC = tdparser.c lrparser.c

parser.o: parser.h $(PARSER_SRC)
	@printf '\CC\t$@\n'
	@cat $(PARSER_SRC) | gcc -x c -c -o parser.o -

grammar_test: grammar_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ grammar_test.c $(OBJECTS)

td_test: td_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ td_test.c $(OBJECTS)

lr_test: lr_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ lr_test.c $(OBJECTS)

gen_test: gen_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ gen_test.c $(OBJECTS)

check: grammar_test td_test lr_test gen_test
	@./run-tests.sh

clean-tests:
	@rm -f *_test

clean: clean-tests
	@rm -f $(OBJECTS) *.gch a.out
	@rm -rf *.dSYM

.PHONY: clean clean-tests
