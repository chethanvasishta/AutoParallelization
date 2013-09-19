all:
	g++ -c -O3 helper.cpp
	g++ -O3 floyd.cpp helper.o -o floyd -fopenmp
	g++ -O3 pipelined1.cpp helper.o -o pipelined1 -fopenmp

debug:
	g++ -g -c -O3 helper.cpp
	g++ -g floyd.cpp helper.o -o floydD -fopenmp
	g++ -g pipelined1.cpp helper.o -o pipelined1D -fopenmp

clean:
	rm -rf floyd pipelined1 floydD pipelined1D *.o
