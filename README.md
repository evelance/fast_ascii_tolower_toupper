# Fast ASCII tolower / toupper with SSE2

C89 / C++ functions to lowercase/uppercase an ASCII buffer in-place. When compiled
with SSE2 support, they use [Peter Cordes](https://github.com/pcordes)' SIMD case flip explained [here](https://stackoverflow.com/a/37151084).

License: Public domain
