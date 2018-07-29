SRC=src/*

all:
	g++ --std=c++11 -o main $(SRC) -I include/
clean:
	rm *o main
