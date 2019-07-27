#sds

# test: test.o sds.o
# 	g++ sds.o test.o -o test

# test.o: testsds.cpp
# 	g++ -c testsds.cpp -o test.o

# sds.o: sds.cpp
# 	g++ -c sds.cpp -o sds.o


#intset

test: test.o intset.o
	g++ intset.o test.o -o test

test.o: testintset.cpp
	g++ -c testintset.cpp -o test.o

intset.o: intset.cpp
	g++ -c intset.cpp -o intset.o




clean:
	rm *.o & rm test
