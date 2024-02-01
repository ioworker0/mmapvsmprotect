#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define MB (1024 * 1024)

int main() {
    // mmap a 2MB anonymous page
    void *mem = mmap(NULL, 2 * MB, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Set the first two pages to 1
    memset(mem, 1, 2 * PAGE_SIZE);

    // Mark the 2MB region as a huge page
    if (madvise(mem, 2 * MB, MADV_HUGEPAGE) == -1) {
        perror("madvise");
        exit(EXIT_FAILURE);
    }

    // Free the first page using MADV_FREE
    if (madvise(mem, PAGE_SIZE, MADV_FREE) == -1) {
        perror("madvise");
        exit(EXIT_FAILURE);
    }

    // Sleep for 10 seconds to allow time for the operations to take effect
    sleep(10);

    // Print the process's smaps
    FILE *smaps_file = fopen("/proc/self/smaps", "r");
    if (smaps_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char c;
    while ((c = fgetc(smaps_file)) != EOF) {
        putchar(c);
    }

    // Check if the first byte of the first page is 1
    if (*((char *)mem) == 1) {
        // If true, exit with status 2
        exit(2);
    }

    // Clean up and exit
    munmap(mem, 2 * MB);
    fclose(smaps_file);

    return 0;
}
