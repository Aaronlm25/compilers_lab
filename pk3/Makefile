CC = gcc
CXX = g++
CFLAGS = -std=c11 -O2 -Wall -Wextra
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

BIN = pk3
OBJS = regex.o nfa.o dfa.o minimizacion.o main.o

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

regex.o: regex.c regex.h
	$(CC) $(CFLAGS) -c regex.c -o regex.o

nfa.o: nfa.c nfa.h regex.h
	$(CC) $(CFLAGS) -c nfa.c -o nfa.o

dfa.o: dfa.cpp dfa.h nfa.h
	$(CXX) $(CXXFLAGS) -c dfa.cpp -o dfa.o

minimizacion.o: minimizacion.cpp dfa.h
	$(CXX) $(CXXFLAGS) -c minimizacion.cpp -o minimizacion.o

main.o: main.cpp dfa.h nfa.h regex.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean
