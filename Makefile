SRC=src/*

all:
	g++ --std=c++11 -o main $(SRC)
	./main input/dorian_gray_5 > debug
clean:
	rm *o main
