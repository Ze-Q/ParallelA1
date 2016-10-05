CC=gcc
CFLAGS=-std=c99 -Wall -fopenmp

all: convolve

test_equality: test_equality.c
	$(CC) $(CFLAGS) -o test_equality test_equality.c lodepng.c -lm

convolve: convolve.c wm.h
	$(CC) $(CFLAGS) -o convolve convolve.c lodepng.c 

test_convolve: test_equality convolve
	./convolve test.png test_convolve_output.png 4
	./test_equality test_convolve.png test_convolve_output.png

bench_convolve:
	time ./convolve test.png test_convolve_output.png $(NUM_OF_THREADS)