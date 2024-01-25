#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <limits.h>
#include <time.h>

#define PAGE_SIZE 4096
#define NUM_ITERATIONS 1000

void measure_mmap_time() {
    struct timespec start, end;
    long min_nanos = LONG_MAX;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {

        // mmap a 2MB anonymous page
        void *ptr = mmap(NULL, 2 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        // Access each page in the allocated memory
        for (size_t i = 0; i < 512; i += PAGE_SIZE) {
            // Write a value to each page
            *(char *)(ptr + i) = 'A';
        }

        clock_gettime(CLOCK_MONOTONIC, &start);

        // Release memory
        ptr = mmap(ptr, 2 * 1024 * 1024, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
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

        // Unmap the memory
        if (munmap(ptr, 2 * 1024 * 1024) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
    }

    // Print the minimum time across all iterations
    printf("Elapsed Time for mmap:             %ld nanoseconds\n", min_nanos);
}

void measure_mprotect_time() {
    struct timespec start, end;
    long min_nanos = LONG_MAX;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        
        // mmap a 2MB anonymous page with no permissions
        void *ptr = mmap(NULL, 2 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        // Access each page in the allocated memory
        for (size_t i = 0; i < 512; i += PAGE_SIZE) {
            // Write a value to each page
            *(char *)(ptr + i) = 'A';
        }

        clock_gettime(CLOCK_MONOTONIC, &start);

        // Change the permissions to none
        if (mprotect(ptr, 2 * 1024 * 1024, PROT_NONE) == -1) {
            perror("mprotect");
            exit(EXIT_FAILURE);
        }

        // Release memory
        if (madvise(ptr, 2 * 1024 * 1024, MADV_DONTNEED) == -1) {
            perror("madvise");
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

        // Unmap the memory
        if (munmap(ptr, 2 * 1024 * 1024) == -1) {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
    }

    // Print the minimum time across all iterations
    printf("Elapsed Time for mprotect+madvise: %ld nanoseconds\n", min_nanos);
}

int main() {
    measure_mprotect_time();
    measure_mmap_time();
    return 0;
}
