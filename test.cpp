#include <chrono>
#include <random>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <immintrin.h>
using namespace std;
using namespace chrono;

#include "asciitolower_upper.c"

static char* dangerous_buffer(size_t len, int protect_last, void*& mmptr, size_t& mmlen)
{
    #ifndef PAGE_SIZE
        #define PAGE_SIZE 4096
    #endif
    mmlen = (len &~ (PAGE_SIZE - 1)) + ((len % PAGE_SIZE) ? 2 : 1) * PAGE_SIZE; // One extra page
    mmptr = mmap(NULL, mmlen, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (mmptr == MAP_FAILED) {
        perror("mmap in dangerous_buffer");
        exit(-1);
    }
    if (mprotect((char*)mmptr + mmlen - PAGE_SIZE, PAGE_SIZE, protect_last)) { // Protect last page
        perror("mprotect in dangerous_buffer");
        exit(-1);
    }
    // Directly after the user pointer is the protected page
    return (char*)mmptr + ((len % PAGE_SIZE) ? (PAGE_SIZE - (len % PAGE_SIZE)) : 0);
}

int main()
{
    const size_t megabytes = 1024;
    printf(" * Testing asciitolower/asciitoupper, 2x%zu megabytes buffer...\n", megabytes);
    // Setup random testing
    #ifdef SEED
        unsigned long seed = SEED;
    #else
        unsigned long seed = high_resolution_clock::now().time_since_epoch().count();
        printf(" * Seed = %ld\n", seed);
    #endif
    default_random_engine dre { seed };
    #define RI(min,max) (uniform_int_distribution<size_t>(min, max))(dre)
    // Allocate buffers
    const size_t len = 1024 * 1024 * megabytes;
    void* mmptr1; size_t mmlen1;
    void* mmptr2; size_t mmlen2;
    char* buf1 = dangerous_buffer(len, PROT_NONE, mmptr1, mmlen1);
    char* buf2 = dangerous_buffer(len, PROT_NONE, mmptr2, mmlen2);
    printf(" * Setting up %.2f GB of random data...\n", megabytes / 1024.0);
    for (size_t i = 0; i <= len - 4; i += 4) {
        uint32_t r = dre();
        buf1[i + 0] = (uint8_t)(r >> 8 * 3);
        buf1[i + 1] = (uint8_t)(r >> 8 * 2);
        buf1[i + 2] = (uint8_t)(r >> 8 * 1);
        buf1[i + 3] = (uint8_t)(r >> 8 * 0);
    }
    {
        // Single step test / asciitolower
        memcpy(buf2, buf1, len);
        auto t1 = high_resolution_clock::now();
        asciitolower(buf2, len);
        double ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
        printf(" * asciitolower single step: %.2fms (%.2f gigabytes/second)\n", ms, (megabytes * 1024.0) / (ms * 1000.0));
        for (size_t i = 0; i < len; ++i) {
            if (buf2[i] != ((buf1[i] >= 'A' && buf1[i] <= 'Z') ? buf1[i] ^ 0x20 : buf1[i])) {
                fprintf(stderr, "Error: Invalid result\n");
                return 1;
            }
        }
        // Multi step test / asciitolower
        memcpy(buf2, buf1, len);
        size_t off = 0, steps = 0;
        t1 = high_resolution_clock::now();
        while (off < len) {
            size_t chunk = RI(0, 64);
            if (off + chunk > len)
                chunk = len - off;
            asciitolower(buf2 + off, chunk);
            off += chunk;
            ++steps;
        }
        ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
        printf(" * asciitolower multi step: %.2fms (%.2f gigabytes/second, %zu steps)\n", ms, (megabytes * 1024.0) / (ms * 1000.0), steps);
        for (size_t i = 0; i < len; ++i) {
            if (buf2[i] != ((buf1[i] >= 'A' && buf1[i] <= 'Z') ? buf1[i] ^ 0x20 : buf1[i])) {
                fprintf(stderr, "Error: Invalid result. Expected \n");
                return 1;
            }
        }
    }
    {
        // Single step test / asciitoupper
        memcpy(buf2, buf1, len);
        auto t1 = high_resolution_clock::now();
        asciitoupper(buf2, len);
        double ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
        printf(" * asciitoupper single step: %.2fms (%.2f gigabytes/second)\n", ms, (megabytes * 1024.0) / (ms * 1000.0));
        for (size_t i = 0; i < len; ++i) {
            if (buf2[i] != ((buf1[i] >= 'a' && buf1[i] <= 'z') ? buf1[i] ^ 0x20 : buf1[i])) {
                fprintf(stderr, "Error: Invalid result\n");
                return 1;
            }
        }
        // Multi step test / asciitoupper
        memcpy(buf2, buf1, len);
        size_t off = 0, steps = 0;
        t1 = high_resolution_clock::now();
        while (off < len) {
            size_t chunk = RI(0, 64);
            if (off + chunk > len)
                chunk = len - off;
            asciitoupper(buf2 + off, chunk);
            off += chunk;
            ++steps;
        }
        ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
        printf(" * asciitoupper multi step: %.2fms (%.2f gigabytes/second, %zu steps)\n", ms, (megabytes * 1024.0) / (ms * 1000.0), steps);
        for (size_t i = 0; i < len; ++i) {
            if (buf2[i] != ((buf1[i] >= 'a' && buf1[i] <= 'z') ? buf1[i] ^ 0x20 : buf1[i])) {
                fprintf(stderr, "Error: Invalid result. Expected \n");
                return 1;
            }
        }
    }
    printf(" * Success.\n");
    munmap(mmptr1, mmlen1);
    munmap(mmptr2, mmlen2);
}

