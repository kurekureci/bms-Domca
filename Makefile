CC=g++
CFLAGS= -Wall -std=c++11 -pedantic -g

p1: p1.cpp
	$(CC) $(CFLAGS) p1.cpp -o p1

clean:
	rm -f p1