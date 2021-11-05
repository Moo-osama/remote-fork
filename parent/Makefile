output: main.o myfork.o
	g++ main.o myfork.o -o run

main.o: main.cpp
	g++ -c main.cpp

myfork.o: myfork.cpp myfork.h
	g++ -c myfork.cpp

clean:
	rm *.o run