CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-function -I. -Ibuild
OBJS     = build/parser.tab.o build/lex.yy.o build/ast.o build/symtab.o build/semantic.o build/main.o

nova: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

build/parser.tab.cpp build/parser.tab.hpp: parser.y ast.hpp
	mkdir -p build
	bison -d -o build/parser.tab.cpp parser.y

build/lex.yy.cpp: scanner.l build/parser.tab.hpp
	flex -o $@ scanner.l

build/%.o: build/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: %.cpp ast.hpp symtab.hpp semantic.hpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build nova
.PHONY: clean
