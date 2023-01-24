CC = cc -g
YACC = yacc

HEADERS = util.h table.h grammar.h parser.h gen.h
OBJECTS = util.o table.o grammar.o parser.o gen.o

GRAMMAR_INC = grammar_parse.c grammar_util.c grammar_lr.c

YACCGEN = yacc.c yacc.h

yacc: yacc.o lex.o $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ main.c yacc.o lex.o $(OBJECTS)

yacc.o: $(YACCGEN)
	@printf '\CC\t$@\n'
	@$(CC) -c yacc.c

$(YACCGEN): yaccgen
	@printf 'GEN\t%s\n' "$(YACCGEN)"
	@./$^ $(YACCGEN)

yaccgen: yaccgen.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ yaccgen.c $(OBJECTS)

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

lex.o: lex.c yacc.h
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

OLDYACCGEN = oy_test.tab.c oy_test.tab.h

oy_test: $(OLDYACCGEN) $(HEADERS) $(OBJECTS) oy_lex.o
	@printf '\CC\t$@\n'
	@$(CC) -o $@ oy_test.tab.c $(OBJECTS) oy_lex.o

oy_lex.o: oy_lex.c $(OLDYACCGEN)
	@printf '\CC\t$@\n'
	@$(CC) -c $^

oy_lex.c: lex.c
	@printf '\GEN\toy_lex.c\n'
	@sed 's/yacc.h/oy_test.tab.h/g' $^ > $@

$(OLDYACCGEN): oldyacc_test.y
	@printf '\YACC\t$@\n'
	@$(YACC) -v -d -b oy_test oldyacc_test.y

check: grammar_test gen_test lex_test
	@./run-tests.sh

clean-tests:
	@rm -f *_test

clean: clean-tests
	@rm -f $(OBJECTS) *.gch a.out yacc.o $(YACCGEN) yaccgen yacc
	@rm -f $(OLDYACCGEN) oy_lex.*
	@rm -rf *.dSYM

.PHONY: clean clean-tests
