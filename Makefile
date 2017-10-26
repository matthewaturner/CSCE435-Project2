all: qsort1 qsort2 bucket

qsort1: qsort1.c
	icc -fopenmp -O3 qsort1.c -lm -ldl -lrt -o qsort1
	./qsort1 1000

qsort2: qsort2.c
	icc -fopenmp -O3 qsort2.c -lm -ldl -lrt -o qsort2

bucket: bucket.c
	icc -fopenmp -O3 bucket.c -lm -ldl -lrt -o bucket

run: qsort1 qsort2 bucket
	./qsort1 100000000
	./qsort2 100000000
	./bucket 100000000
