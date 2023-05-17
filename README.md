# A Self-hosting Compiler-Compiler

_This repo contains an implementation of Yacc that generates its own parser, but
is extremely limited in its feature set. I was so excited when it worked, that I
moved on to the next thing without properly documenting it. I hope to document
it fully at some point in the next few weeks._

By _self-hosting_ I mean that the parser for the [Yacc input
langauge](https://www.cs.utexas.edu/users/novak/yaccpaper.htm) is hardcoded
within the AST I use, and it is from this hardcoded AST that the Yacc program
itself is generated. You can find the hardcoded AST [here](yaccgen.c).

This works because any Yacc implementation must have two main components:

1. A parser to take the parser description provided in a `*.y` file and produce
   an AST

2. A generator to take this AST and produce (the C source for) a parser.

Hardcoding the AST for the Yacc language, though unnecessary, leads to an
implementation that is beautiful (at least conceptually) and recursive and feels
paradoxical.

You can test the program as follows (but remember! the implementation is very
bare-bones):

```bash
make
./yacc examples/calc.y
```

and you should get something like

```
line   → expr \n
expr   → expr + term | term
term   → term * factor | factor
factor → ( expr ) | DIGIT
```

corresponding to the grammar in [calc.y](examples/calc.y). By careful
examination of the [Makefile](Makefile) you can confirm that I'm using the
generated parser only.
