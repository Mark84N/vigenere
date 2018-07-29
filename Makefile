SRC=src/*

all:
	g++ --std=c++11 -o main $(SRC)
clean:
	rm *o main
