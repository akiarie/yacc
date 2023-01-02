CC = cc -g

HEADERS = util.h table.h grammar.h parser.h
OBJECTS = util.o table.o grammar.o parser.o

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

grammar_test: grammar_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ grammar_test.c $(OBJECTS)

parser_test: parser_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ parser_test.c $(OBJECTS)

check: grammar_test parser_test
	@./run-tests.sh

clean-tests:
	@rm -f *_test

clean: clean-tests
	@rm -f $(OBJECTS) *.gch a.out
	@rm -rf *.dSYM

.PHONY: clean clean-tests
