CC     = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -Wno-unused-function -I. -Ibuild
OBJS   = build/parser.tab.o build/lex.yy.o build/ast.o build/symtab.o build/semantic.o build/main.o

nova: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

build/parser.tab.c build/parser.tab.h: parser.y ast.h
	mkdir -p build
	bison -d -o build/parser.tab.c parser.y

build/lex.yy.c: scanner.l build/parser.tab.h
	flex -o $@ scanner.l

build/%.o: build/%.c
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.c ast.h symtab.h semantic.h
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build nova
.PHONY: clean
