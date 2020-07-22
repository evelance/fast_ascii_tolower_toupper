#include <chrono>
#include <random>
#include <thread>
#include <cstring>
#include <iostream>
using namespace std;
using namespace chrono;

#include "asciitolower_upper.c"

int main()
{
    const size_t megabytes = 1024;
    cout << " * Testing asciitolower/asciitoupper, 2x" << megabytes << " megabytes buffer..." << endl;
    // Setup random testing
    #ifdef SEED
        unsigned long seed = SEED;
    #else
        unsigned long seed = high_resolution_clock::now().time_since_epoch().count();
        cout << " * Seed = " << seed << endl;
    #endif
    default_random_engine dre { seed };
    #define RI(min,max) (uniform_int_distribution<size_t>(min, max))(dre)
    // Allocate buffers
    const size_t len = 1024 * 1024 * megabytes;
    char* buf1 = new char[len];
    char* buf2 = new char[len];
    cout << " * Setting up " << (megabytes / 1024.0) << " GB of random data..." << endl;
    for (size_t i = 0; i <= len - 4; i += 4) {
        uint32_t r = dre();
        buf1[i + 0] = (uint8_t)(r >> 8 * 3);
        buf1[i + 1] = (uint8_t)(r >> 8 * 2);
        buf1[i + 2] = (uint8_t)(r >> 8 * 1);
        buf1[i + 3] = (uint8_t)(r >> 8 * 0);
    }
    double ms;
    size_t off, steps;
    auto t1 = high_resolution_clock::now();
    this_thread::sleep_for(seconds(3));
    // Single step test / asciitolower
    memcpy(buf2, buf1, len);
    t1 = high_resolution_clock::now();
    asciitolower(buf2, len);
    ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
    cout << " * asciitolower single step: " << ms << "ms (" << ((megabytes * 1024.0) / (ms * 1000.0)) << " gigabytes/second)" << endl;
    for (size_t i = 0; i < len; ++i) {
        if (buf2[i] != ((buf1[i] >= 'A' && buf1[i] <= 'Z') ? buf1[i] ^ 0x20 : buf1[i])) {
            fprintf(stderr, "Error: Invalid result.\n");
            return 1;
        }
    }
    this_thread::sleep_for(seconds(3));
    // Multi step test / asciitolower
    memcpy(buf2, buf1, len);
    t1 = high_resolution_clock::now();
    for (off = 0, steps = 0; off < len; ++steps) {
        size_t chunk = RI(0, 64);
        if (off + chunk > len)
            chunk = len - off;
        asciitolower(buf2 + off, chunk);
        off += chunk;
    }
    ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
    cout << " * asciitolower multi step: " << ms << "ms (" << ((megabytes * 1024.0) / (ms * 1000.0)) << " gigabytes/second, "
         << steps << " steps)" << endl;
    for (size_t i = 0; i < len; ++i) {
        if (buf2[i] != ((buf1[i] >= 'A' && buf1[i] <= 'Z') ? buf1[i] ^ 0x20 : buf1[i])) {
            fprintf(stderr, "Error: Invalid result.\n");
            return 1;
        }
    }
    this_thread::sleep_for(seconds(3));
    // Single step test / asciitoupper
    memcpy(buf2, buf1, len);
    t1 = high_resolution_clock::now();
    asciitoupper(buf2, len);
    ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
    cout << " * asciitoupper single step: " << ms << "ms (" << ((megabytes * 1024.0) / (ms * 1000.0)) << " gigabytes/second)" << endl;
    for (size_t i = 0; i < len; ++i) {
        if (buf2[i] != ((buf1[i] >= 'a' && buf1[i] <= 'z') ? buf1[i] ^ 0x20 : buf1[i])) {
            fprintf(stderr, "Error: Invalid result.\n");
            return 1;
        }
    }
    this_thread::sleep_for(seconds(3));
    // Multi step test / asciitoupper
    memcpy(buf2, buf1, len);
    t1 = high_resolution_clock::now();
    for (off = 0, steps = 0; off < len; ++steps) {
        size_t chunk = RI(0, 64);
        if (off + chunk > len)
            chunk = len - off;
        asciitoupper(buf2 + off, chunk);
        off += chunk;
    }
    ms = (double(duration_cast<nanoseconds>(high_resolution_clock::now() - t1).count()) / 1000000.0);
    cout << " * asciitoupper multi step: " << ms << "ms (" << ((megabytes * 1024.0) / (ms * 1000.0)) << " gigabytes/second, "
         << steps << " steps)" << endl;
    for (size_t i = 0; i < len; ++i) {
        if (buf2[i] != ((buf1[i] >= 'a' && buf1[i] <= 'z') ? buf1[i] ^ 0x20 : buf1[i])) {
            fprintf(stderr, "Error: Invalid result.\n");
            return 1;
        }
    }
    cout << " * Success." << endl;
    delete[] buf1;
    delete[] buf2;
}

