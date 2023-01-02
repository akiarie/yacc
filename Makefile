CC = cc -g

HEADERS = util.h table.h parser.h
OBJECTS = util.o table.o parser.o

util.o: util.h util.c
	@printf '\CC\t$@\n'
	@$(CC) -c util.c

table.o: table.h table.c
	@printf '\CC\t$@\n'
	@$(CC) -c table.c

parser_test: parser_test.c $(HEADERS) $(OBJECTS)
	@printf '\CC\t$@\n'
	@$(CC) -o $@ parser_test.c $(OBJECTS)

check: parser_test
	@./run-tests.sh

clean-tests:
	@rm -f *_test

clean: clean-tests
	@rm -f $(OBJECTS) *.gch a.out
	@rm -rf *.dSYM

.PHONY: clean clean-tests
