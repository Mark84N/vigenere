CC=g++
OBJS=src/*
CXX_FLAGS=-I include --std=c++11 -Wall -Werror -Wextra

all: vigenere

vigenere:
	$(CC) -o vigenere $(OBJS) $(CXX_FLAGS)

clean:
	rm vigenere 2>/dev/null

