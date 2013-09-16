all:
	g++ -g floyd.cpp -o floyd
	g++ -O3 pipelined1.cpp -o pipelined1

clean:
	rm -rf floyd pipelined1
