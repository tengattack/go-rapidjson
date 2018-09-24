
build: cgo
	g++ -I. -O3 -c rapidJSON_api.cpp -std=c++11
	gcc -I. -O3 -c lib/gotypes/types.c
	gcc -I. -O3 -c _obj/_cgo_export.c
	ar -crs librapidJSON_api.a rapidJSON_api.o types.o _cgo_export.o

cgo:
	go tool cgo ../gotypes.go
