#pragma once
/**
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/
#include <cybozu/inttype.hpp>
#include <stdio.h>
#include <assert.h>
#ifdef _WIN32
	#include <winsock2.h>
	#include <intrin.h>
#else
    // ABr: cybozu does not support ARM natively
    // ABr: see https://clang.llvm.org/compatibility.html#vector_builtins
    #if defined(__arm__)
        // ABr: 32-bit is not supported :(
        #define UINT32VEC_UNSUPPORTED
    #else
        #if defined(__aarch64__)
            // ABr: unclear if we should target 128 / 256 vectors
            #define __AVX2__

            // ABr: see simd readme - target the *original architecture*
            #ifdef __AVX2__
                #define SIMDE_X86_AVX_ENABLE_NATIVE_ALIASES
                #define SIMDE_X86_AVX2_ENABLE_NATIVE_ALIASES
                #include <simde/x86/avx2.h>
            #else
                #define SIMDE_X86_AVX_ENABLE_NATIVE_ALIASES
                #include <simde/x86/avx.h>
            #endif

            // ABr: clang fails on a couple of simd macros.
            #ifdef __clang__
                #define ARM_HANDLE_BUGGY_CLANG
            #endif
        #else
            #ifdef __linux__
                #include <x86intrin.h>
            #else
                #include <emmintrin.h>
            #endif
        #endif
    #endif
#endif

template<size_t size>
struct Uint32VecT {
};

#if !defined(UINT32VEC_UNSUPPORTED)
#ifdef __AVX2__
#define UINT32VEC_USE_AVX2
template<>
struct Uint32VecT<8> {
	static const int size = 8;
	__m256i x_;
	Uint32VecT()
	{
	}
	Uint32VecT(__m256i x)
		: x_(x)
	{
	}
	Uint32VecT(uint32_t x)
		: x_(_mm256_set1_epi32(x))
	{
	}
	// m = [x7:x6:x5:x4:x3:x2:x1:x0]
	Uint32VecT(uint32_t x7, uint32_t x6, uint32_t x5, uint32_t x4, uint32_t x3, uint32_t x2, uint32_t x1, uint32_t x0)
		: x_(_mm256_set_epi32(x7, x6, x5, x4, x3, x2, x1, x0))
	{
	}
	// m = [x7:x6:x5:x4:x3:x2:x1:x0]
	void set(uint32_t x7, uint32_t x6, uint32_t x5, uint32_t x4, uint32_t x3, uint32_t x2, uint32_t x1, uint32_t x0)
	{
		x_ = _mm256_set_epi32(x7, x6, x5, x4, x3, x2, x1, x0);
	}
	void clear()
	{
		*this = _mm256_setzero_si256();
	}
	void put(const char *msg = 0) const
	{
		uint32_t v[8];
		memcpy(&v, &x_, sizeof(v));
		if (msg) printf("%s", msg);
		printf("%08x:%08x:%08x:%08x", v[3], v[2], v[1], v[0]);
		if (msg) putchar('\n');
	}
};

#ifdef ARM_HANDLE_BUGGY_CLANG
/**
 ABr: clang cannot an int as a compile-time constant - this is a workaround for shift left.
 See https://github.com/VectorCamp/vectorscan/issues/21
 */
inline int32x4_t
_arm_avx2_34_builtin_neon_vshlq_n_v(
                          int8x16_t a,
                          int n,
                          int unused
)
{
#define ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34 34
    switch(n & 31) {
        case 0: return __builtin_neon_vshlq_n_v(a, 0, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 1: return __builtin_neon_vshlq_n_v(a, 1, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 2: return __builtin_neon_vshlq_n_v(a, 2, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 3: return __builtin_neon_vshlq_n_v(a, 3, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 4: return __builtin_neon_vshlq_n_v(a, 4, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 5: return __builtin_neon_vshlq_n_v(a, 5, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 6: return __builtin_neon_vshlq_n_v(a, 6, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 7: return __builtin_neon_vshlq_n_v(a, 7, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 8: return __builtin_neon_vshlq_n_v(a, 8, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 9: return __builtin_neon_vshlq_n_v(a, 9, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 10: return __builtin_neon_vshlq_n_v(a, 10, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 11: return __builtin_neon_vshlq_n_v(a, 11, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 12: return __builtin_neon_vshlq_n_v(a, 12, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 13: return __builtin_neon_vshlq_n_v(a, 13, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 14: return __builtin_neon_vshlq_n_v(a, 14, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 15: return __builtin_neon_vshlq_n_v(a, 15, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 16: return __builtin_neon_vshlq_n_v(a, 16, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 17: return __builtin_neon_vshlq_n_v(a, 17, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 18: return __builtin_neon_vshlq_n_v(a, 18, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 19: return __builtin_neon_vshlq_n_v(a, 19, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 20: return __builtin_neon_vshlq_n_v(a, 20, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 21: return __builtin_neon_vshlq_n_v(a, 21, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 22: return __builtin_neon_vshlq_n_v(a, 22, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 23: return __builtin_neon_vshlq_n_v(a, 23, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 24: return __builtin_neon_vshlq_n_v(a, 24, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 25: return __builtin_neon_vshlq_n_v(a, 25, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 26: return __builtin_neon_vshlq_n_v(a, 26, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 27: return __builtin_neon_vshlq_n_v(a, 27, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 28: return __builtin_neon_vshlq_n_v(a, 28, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 29: return __builtin_neon_vshlq_n_v(a, 29, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        case 30: return __builtin_neon_vshlq_n_v(a, 30, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
        default: return __builtin_neon_vshlq_n_v(a, 31, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHLQ_N_V_34);
    }
}

/**
 ABr: clang cannot an int as a compile-time constant - this is a workaround for shift right.
 See https://github.com/VectorCamp/vectorscan/issues/21
 */
inline int32x4_t
_arm_avx2_50_builtin_neon_vshrq_n_v(
                          int8x16_t a,
                          int n,
                          int unused
)
{
#define ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50 50
    switch(n & 31) {
        // ABr: "1" is not a typo - zero is out of range
        case 0: return __builtin_neon_vshrq_n_v(a, 1, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 1: return __builtin_neon_vshrq_n_v(a, 1, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 2: return __builtin_neon_vshrq_n_v(a, 2, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 3: return __builtin_neon_vshrq_n_v(a, 3, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 4: return __builtin_neon_vshrq_n_v(a, 4, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 5: return __builtin_neon_vshrq_n_v(a, 5, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 6: return __builtin_neon_vshrq_n_v(a, 6, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 7: return __builtin_neon_vshrq_n_v(a, 7, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 8: return __builtin_neon_vshrq_n_v(a, 8, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 9: return __builtin_neon_vshrq_n_v(a, 9, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 10: return __builtin_neon_vshrq_n_v(a, 10, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 11: return __builtin_neon_vshrq_n_v(a, 11, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 12: return __builtin_neon_vshrq_n_v(a, 12, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 13: return __builtin_neon_vshrq_n_v(a, 13, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 14: return __builtin_neon_vshrq_n_v(a, 14, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 15: return __builtin_neon_vshrq_n_v(a, 15, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 16: return __builtin_neon_vshrq_n_v(a, 16, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 17: return __builtin_neon_vshrq_n_v(a, 17, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 18: return __builtin_neon_vshrq_n_v(a, 18, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 19: return __builtin_neon_vshrq_n_v(a, 19, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 20: return __builtin_neon_vshrq_n_v(a, 20, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 21: return __builtin_neon_vshrq_n_v(a, 21, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 22: return __builtin_neon_vshrq_n_v(a, 22, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 23: return __builtin_neon_vshrq_n_v(a, 23, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 24: return __builtin_neon_vshrq_n_v(a, 24, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 25: return __builtin_neon_vshrq_n_v(a, 25, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 26: return __builtin_neon_vshrq_n_v(a, 26, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 27: return __builtin_neon_vshrq_n_v(a, 27, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 28: return __builtin_neon_vshrq_n_v(a, 28, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 29: return __builtin_neon_vshrq_n_v(a, 29, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        case 30: return __builtin_neon_vshrq_n_v(a, 30, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
        default: return __builtin_neon_vshrq_n_v(a, 31, ARM_HANDLE_BUGGY_CLANG_BUILTIN_NEON_VSHRQ_N_V_50);
    }
}

#endif

inline Uint32VecT<8> operator<<(const Uint32VecT<8>& a, const int n)
{
#ifdef ARM_HANDLE_BUGGY_CLANG
    // ABr: extracted from preprocessor invoking _mm256_slli_epi32
    return simde_mm256_set_m128i(((((n)) <= 0) ? (simde_mm256_extracti128_si256(a.x_, 1)) : simde__m128i_from_neon_i32( (((n)) > 31) ? vandq_s32(simde__m128i_to_neon_i32(simde_mm256_extracti128_si256(a.x_, 1)), vdupq_n_s32(0)) : __extension__ ({ int32x4_t __s0 = simde__m128i_to_neon_i32(simde_mm256_extracti128_si256(a.x_, 1)); int32x4_t __ret; __ret = (int32x4_t) _arm_avx2_34_builtin_neon_vshlq_n_v((int8x16_t)__s0, n, 34); __ret; }))), ((((n)) <= 0) ? (simde_mm256_extracti128_si256(a.x_, 0)) : simde__m128i_from_neon_i32( (((n)) > 31) ? vandq_s32(simde__m128i_to_neon_i32(simde_mm256_extracti128_si256(a.x_, 0)), vdupq_n_s32(0)) : __extension__ ({ int32x4_t __s0 = simde__m128i_to_neon_i32(simde_mm256_extracti128_si256(a.x_, 0)); int32x4_t __ret; __ret = (int32x4_t) _arm_avx2_34_builtin_neon_vshlq_n_v((int8x16_t)__s0, (((n)) & 31), 34); __ret; }))));
#else
    return _mm256_slli_epi32(a.x_, n);
#endif
}

inline Uint32VecT<8> operator>>(const Uint32VecT<8>& a, const int n)
{
#ifdef ARM_HANDLE_BUGGY_CLANG
    // ABr: extracted from preprocessor invoking _mm256_srli_epi32
    return simde_mm256_set_m128i( ((((n)) <= 0) ? (simde_mm256_extracti128_si256(a.x_, 1)) : simde__m128i_from_neon_u32( (((n)) > 31) ? vandq_u32(simde__m128i_to_neon_u32(simde_mm256_extracti128_si256(a.x_, 1)), vdupq_n_u32(0)) : __extension__ ({ uint32x4_t __s0 = simde__m128i_to_neon_u32(simde_mm256_extracti128_si256(a.x_, 1)); uint32x4_t __ret; __ret = (uint32x4_t) _arm_avx2_50_builtin_neon_vshrq_n_v((int8x16_t)__s0, (((n)) & 31) | ((((n)) & 31) == 0), 50); __ret; }))), ((((n)) <= 0) ? (simde_mm256_extracti128_si256(a.x_, 0)) : simde__m128i_from_neon_u32( (((n)) > 31) ? vandq_u32(simde__m128i_to_neon_u32(simde_mm256_extracti128_si256(a.x_, 0)), vdupq_n_u32(0)) : __extension__ ({ uint32x4_t __s0 = simde__m128i_to_neon_u32(simde_mm256_extracti128_si256(a.x_, 0)); uint32x4_t __ret; __ret = (uint32x4_t) _arm_avx2_50_builtin_neon_vshrq_n_v((int8x16_t)__s0, (((n)) & 31) | ((((n)) & 31) == 0), 50); __ret; }))));
#else
	return _mm256_srli_epi32(a.x_, n);
#endif
}

inline Uint32VecT<8> operator+(const Uint32VecT<8>& a, const Uint32VecT<8>& b)
{
	return _mm256_add_epi32(a.x_, b.x_);
}

// return (~a) & b
inline Uint32VecT<8> andn(const Uint32VecT<8>& a, const Uint32VecT<8>& b)
{
	return _mm256_andnot_si256(a.x_, b.x_);
}

inline Uint32VecT<8> operator|(const Uint32VecT<8>& a, const Uint32VecT<8>& b)
{
	return _mm256_or_si256(a.x_, b.x_);
}

inline Uint32VecT<8> operator&(const Uint32VecT<8>& a, const Uint32VecT<8>& b)
{
	return _mm256_and_si256(a.x_, b.x_);
}

inline Uint32VecT<8> operator^(const Uint32VecT<8>& a, const Uint32VecT<8>& b)
{
	return _mm256_xor_si256(a.x_, b.x_);
}

typedef Uint32VecT<8> Uint32Vec;

#else

template<>
struct Uint32VecT<4> {
	static const int size = 4;
	__m128i x_;
	Uint32VecT()
	{
	}
	Uint32VecT(__m128i x)
		: x_(x)
	{
	}
	Uint32VecT(uint32_t x)
		: x_(_mm_set1_epi32(x))
	{
	}
	// m = [x3:x2:x1:x0]
	Uint32VecT(uint32_t x3, uint32_t x2, uint32_t x1, uint32_t x0)
		: x_(_mm_set_epi32(x3, x2, x1, x0))
	{
	}
	// m = [x3:x2:x1:x0]
	void set(uint32_t x3, uint32_t x2, uint32_t x1, uint32_t x0)
	{
		x_ = _mm_set_epi32(x3, x2, x1, x0);
	}
	void clear()
	{
		*this = _mm_setzero_si128();
	}
	void put(const char *msg = 0) const
	{
		uint32_t v[4];
		memcpy(&v, &x_, sizeof(v));
		if (msg) printf("%s", msg);
		printf("%08x:%08x:%08x:%08x", v[3], v[2], v[1], v[0]);
		if (msg) putchar('\n');
	}
};

inline Uint32VecT<4> operator<<(const Uint32VecT<4>& a, const int n)
{
	return _mm_slli_epi32(a.x_, n);
}
inline Uint32VecT<4> operator>>(const Uint32VecT<4>& a, const int n)
{
	return _mm_srli_epi32(a.x_, n);
}

inline Uint32VecT<4> operator+(const Uint32VecT<4>& a, const Uint32VecT<4>& b)
{
	return _mm_add_epi32(a.x_, b.x_);
}

// return (~a) & b
inline Uint32VecT<4> andn(const Uint32VecT<4>& a, const Uint32VecT<4>& b)
{
	return _mm_andnot_si128(a.x_, b.x_);
}

inline Uint32VecT<4> operator|(const Uint32VecT<4>& a, const Uint32VecT<4>& b)
{
	return _mm_or_si128(a.x_, b.x_);
}

inline Uint32VecT<4> operator&(const Uint32VecT<4>& a, const Uint32VecT<4>& b)
{
	return _mm_and_si128(a.x_, b.x_);
}

inline Uint32VecT<4> operator^(const Uint32VecT<4>& a, const Uint32VecT<4>& b)
{
	return _mm_xor_si128(a.x_, b.x_);
}

typedef Uint32VecT<4> Uint32Vec;
#endif
#endif
