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


rectify: rectify.c 
	$(CC) $(CFLAGS) -o rectify rectify.c lodepng.c 

test_rectify: test_equality rectify
	./rectify test.png test_rectify_output.png 4
	./test_equality test_rectify.png test_rectify_output.png

bench_rectify:
	time ./rectify test.png test_rectify_output.png $(NUM_OF_THREADS)


pool: pool.c 
	$(CC) $(CFLAGS) -o pool pool.c lodepng.c 

test_pool: test_equality pool
	./pool test.png test_pool_output.png 4
	./test_equality test_pool.png test_pool_output.png

bench_pool:
	time ./pool test.png test_pool_output.png $(NUM_OF_THREADS)