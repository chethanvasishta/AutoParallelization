all:
	g++ -g floyd.cpp -o floyd
	g++ -O3 pipelined1.cpp -o pipelined1 -fopenmp

clean:
	rm -rf floyd pipelined1
