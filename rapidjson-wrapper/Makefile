%.o: %.c
	gcc -g -c $<

%.o: %.cpp
	g++ -g -c $<

build:
	g++ -O3 -c rapidJSON_api.cpp
	ar -crs librapidJSON_api.a rapidJSON_api.o
