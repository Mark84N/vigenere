CC=g++
OBJS=src/*
CXX_FLAGS=-I include --std=c++11 

all: vigenere

vigenere:
	$(CC) -o vigenere $(OBJS) $(CXX_FLAGS)

clean:
	rm *o vigenere 2>/dev/null

