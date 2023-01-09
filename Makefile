CC = cc -g

HEADERS = util.h table.h grammar.h parser.h gen.h
OBJECTS = util.o table.o grammar.o parser.o gen.o

GRAMMAR_INC = grammar_parse.c grammar_util.c grammar_lr.c

MAIN = main.c main.h

main: yaccgen
	@printf 'GEN\t%s\n' "$(MAIN)"
	@./$< $(MAIN)

yaccgen: yacc.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ yacc.c $(OBJECTS)

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

lex.o: lex.c main.h
	@printf '\CC\t$@\n'
	@$(CC) -c lex.c

grammar_test: grammar_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ grammar_test.c $(OBJECTS)

gen_test: gen_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ gen_test.c $(OBJECTS)

lex_test: lex_test.c lex.o
	@printf '\CC\t$@\n'
	@$(CC) -o $@ $^

check: grammar_test gen_test lex_test
	@./run-tests.sh

clean-tests:
	@rm -f *_test

clean: clean-tests
	@rm -f $(OBJECTS) *.gch a.out main.c yaccgen yacc
	@rm -rf *.dSYM

.PHONY: clean clean-tests
