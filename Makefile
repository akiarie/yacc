CC = cc -g

HEADERS = util.h maps.h table.h grammar.h parser.h gen.h
OBJECTS = util.o maps.o table.o grammar.o parser.o gen.o

GRAMMAR_INC = grammar_parse.c grammar_util.c grammar_lr.c

util.o: util.h util.c
	@printf '\CC\t$@\n'
	@$(CC) -c util.c

table.o: table.h table.c
	@printf '\CC\t$@\n'
	@$(CC) -c table.c

grammar.o: grammar.h grammar.c $(GRAMMAR_INC)
	@printf '\CC\t$@\n'
	@$(CC) -c grammar.c

parser.o: parser.h parser.c
	@printf '\CC\t$@\n'
	@$(CC) -c parser.c

gen.o: gen.h gen.c
	@printf '\CC\t$@\n'
	@$(CC) -c gen.c

map.o: map.h map.c
	@printf '\CC\t$@\n'
	@$(CC) -c map.c

grammar_test: grammar_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ grammar_test.c $(OBJECTS)

gen_test: gen_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ gen_test.c $(OBJECTS)

check: grammar_test gen_test
	@./run-tests.sh

clean-tests:
	@rm -f *_test

clean: clean-tests
	@rm -f $(OBJECTS) *.gch a.out
	@rm -rf *.dSYM

.PHONY: clean clean-tests
