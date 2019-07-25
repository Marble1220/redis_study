test: test.o sds.o
	g++ sds.o test.o -o test

test.o: test.cpp
	g++ -c test.cpp -o test.o

sds.o: sds.cpp
	g++ -c sds.cpp -o sds.o

clean:
	rm *.o & rm test
