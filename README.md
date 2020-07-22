# Fast ASCII tolower / toupper with SSE2

C89 / C++ functions to lowercase/uppercase an ASCII buffer in-place. When compiled
with SSE2 support, they use [Peter Cordes](https://github.com/pcordes)' SIMD case flip explained [here](https://stackoverflow.com/a/37151084).

License: Public domain

Sample test results with Linux/GCC9/i5-2450M:
```
g++ -std=c++11 -Wall -Wextra -pedantic -O2 -msse2 -o test-sse2 test.cpp -lstdc++
g++ -std=c++11 -Wall -Wextra -pedantic -O2 -mno-sse2 -o test-no-sse2 test.cpp -lstdc++
./test-sse2
 * Testing asciitolower/asciitoupper, 2x1024 megabytes buffer...
 * Seed = 1595440823459653953
 * Setting up 1 GB of random data...
 * asciitolower single step: 143.879ms (7.2879 gigabytes/second)
 * asciitolower multi step: 1913.69ms (0.547935 gigabytes/second, 33555840 steps)
 * asciitoupper single step: 143.865ms (7.28862 gigabytes/second)
 * asciitoupper multi step: 1911.59ms (0.548537 gigabytes/second, 33556068 steps)
 * Success.
./test-no-sse2
 * Testing asciitolower/asciitoupper, 2x1024 megabytes buffer...
 * Seed = 1595440850168201456
 * Setting up 1 GB of random data...
 * asciitolower single step: 1238.26ms (0.846812 gigabytes/second)
 * asciitolower multi step: 2548.54ms (0.411442 gigabytes/second, 33558322 steps)
 * asciitoupper single step: 1274.11ms (0.82299 gigabytes/second)
 * asciitoupper multi step: 2614.18ms (0.40111 gigabytes/second, 33561248 steps)
 * Success.
'''