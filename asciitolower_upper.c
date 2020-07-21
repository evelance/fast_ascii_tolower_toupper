#include <inttypes.h>
#include <immintrin.h>

void asciitolower(char* data, size_t len)
{
    #ifdef __SSE2__
    while (len >= 16) { /* By Peter Cordes */
        __m128i input      = _mm_loadu_si128((__m128i*)data);
        __m128i rangeshift = _mm_sub_epi8(input, _mm_set1_epi8('A' - 128));
        __m128i nomodify   = _mm_cmpgt_epi8(rangeshift, _mm_set1_epi8(25 - 128));
        __m128i flip       = _mm_andnot_si128(nomodify, _mm_set1_epi8(0x20));
        _mm_storeu_si128((__m128i*)data, _mm_xor_si128(input, flip));
        len  -= 16;
        data += 16;
    }
    #endif
    while (len-- > 0) {
        *data += ((unsigned char)(*data - 'A') < 26) << 5;
        ++data;
    }
}

void asciitoupper(char* data, size_t len)
{
    #ifdef __SSE2__
    while (len >= 16) { /* By Peter Cordes */
        __m128i input      = _mm_loadu_si128((__m128i*)data);
        __m128i rangeshift = _mm_sub_epi8(input, _mm_set1_epi8('a' - 128));
        __m128i nomodify   = _mm_cmpgt_epi8(rangeshift, _mm_set1_epi8(25 - 128));
        __m128i flip       = _mm_andnot_si128(nomodify, _mm_set1_epi8(0x20));
        _mm_storeu_si128((__m128i*)data, _mm_xor_si128(input, flip));
        len -= 16;
        data += 16;
    }
    #endif
    while (len-- > 0) {
        *data -= ((unsigned char)(*data - 'a') < 26) << 5;
        ++data;
    }
}

