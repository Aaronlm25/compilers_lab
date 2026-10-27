CXX = g++
LEX = flex
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

BIN = pk6
OBJS = grammar.o scanner.o ast.o parser.o main.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# flex genera C, pero se compila como C++ para usar los tipos de lexer.hpp
scanner.cpp: scanner.l
	$(LEX) -o scanner.cpp scanner.l

scanner.o: scanner.cpp lexer.hpp grammar.hpp
	$(CXX) $(CXXFLAGS) -Wno-unused-function -Wno-sign-compare -c scanner.cpp -o scanner.o

grammar.o: grammar.cpp grammar.hpp
	$(CXX) $(CXXFLAGS) -c grammar.cpp -o grammar.o

ast.o: ast.cpp ast.hpp grammar.hpp
	$(CXX) $(CXXFLAGS) -c ast.cpp -o ast.o

parser.o: parser.cpp parser.hpp ast.hpp lexer.hpp grammar.hpp
	$(CXX) $(CXXFLAGS) -c parser.cpp -o parser.o

main.o: main.cpp parser.hpp lexer.hpp grammar.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

test: $(BIN)
	./run_tests.sh

clean:
	rm -f $(OBJS) $(BIN) scanner.cpp

.PHONY: all clean test
