
build:
	g++ -I. -O3 -c rapidJSON_api.cpp -std=c++11
	gcc -I. -O3 -c lib/gotypes/types.c -std=c99
	gcc -I. -O3 -c lib/gotypes/hash.c
	ar -crs librapidJSON_api.a rapidJSON_api.o types.o hash.o
