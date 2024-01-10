#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <limits.h>
#include <time.h>

#define PAGE_SIZE 4096
#define NUM_ITERATIONS 100000

void measure_mmap_time() {
    struct timespec start, end;
    long min_nanos = LONG_MAX;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {

        // mmap a 4MB anonymous page with no permissions
        void *ptr = mmap(NULL, 4 * 1024 * 1024, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        clock_gettime(CLOCK_MONOTONIC, &start);

        // Change the permissions to read-write using mmap with fixed address
        void *ptr_rw = mmap(ptr, 4 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr_rw == MAP_FAILED) {
            perror("mmap (change permissions)");
            exit(EXIT_FAILURE);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        // Calculate and print the elapsed time for this iteration in nanoseconds
        long seconds = end.tv_sec - start.tv_sec;
        long nanos = (seconds * 1000000000) + (end.tv_nsec - start.tv_nsec);

        // Update the minimum time
        if (nanos < min_nanos) {
            min_nanos = nanos;
        }
    }

    // Print the minimum time across all iterations
    printf("Elapsed Time for mmap:     %ld nanoseconds\n", min_nanos);
}

void measure_mprotect_time() {
    struct timespec start, end;
    long min_nanos = LONG_MAX;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        
        // mmap a 4MB anonymous page with no permissions
        void *ptr = mmap(NULL, 4 * 1024 * 1024, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        clock_gettime(CLOCK_MONOTONIC, &start);

        // Change the permissions to read-write
        if (mprotect(ptr, 4 * 1024 * 1024, PROT_READ | PROT_WRITE) == -1) {
            perror("mprotect");
            exit(EXIT_FAILURE);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        // Calculate and print the elapsed time for this iteration in nanoseconds
        long seconds = end.tv_sec - start.tv_sec;
        long nanos = (seconds * 1000000000) + (end.tv_nsec - start.tv_nsec);

        // Update the minimum time
        if (nanos < min_nanos) {
            min_nanos = nanos;
        }
    }

    // Print the minimum time across all iterations
    printf("Elapsed Time for mprotect: %ld nanoseconds\n", min_nanos);
}

int main() {
    measure_mprotect_time();
    measure_mmap_time();
    return 0;
}
