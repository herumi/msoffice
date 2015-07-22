#pragma once
/**
	@file custom sha1
	@brief simd version of sha1
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/

//#define SHA1_DONT_USE_CYBOZULIB
#ifndef SHA1_DONT_USE_CYBOZULIB
#include <cybozu/endian.hpp>
#endif
#include <assert.h>
#include <string>

#define SHA1_USE_SIMD
#ifdef SHA1_USE_SIMD
#include "uint32vec.hpp"
#endif

#define SHA1_CALC0(f, W, K) \
{ \
	T tmp = S(a, 5) + f(b, c, d) + e + W + K; \
	e = d; \
	d = c; \
	c = S(b, 30); \
	b = a; \
	a = tmp; \
}

#define SHA1_CALC1(i, f, K) SHA1_CALC0(f, W[i], K)

#define SHA1_CALC2(i, f, K) \
	W[i] = S(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1); \
	SHA1_CALC1(i, f, K) \

#define SHA1_LOOP20(loopOne, offset, f, K) \
	loopOne(offset + 0, f, K) \
	loopOne(offset + 1, f, K) \
	loopOne(offset + 2, f, K) \
	loopOne(offset + 3, f, K) \
	loopOne(offset + 4, f, K) \
	loopOne(offset + 5, f, K) \
	loopOne(offset + 6, f, K) \
	loopOne(offset + 7, f, K) \
	loopOne(offset + 8, f, K) \
	loopOne(offset + 9, f, K) \
	loopOne(offset +10, f, K) \
	loopOne(offset +11, f, K) \
	loopOne(offset +12, f, K) \
	loopOne(offset +13, f, K) \
	loopOne(offset +14, f, K) \
	loopOne(offset +15, f, K) \
	loopOne(offset +16, f, K) \
	loopOne(offset +17, f, K) \
	loopOne(offset +18, f, K) \
	loopOne(offset +19, f, K)

#define SHA1_LOOP1(f, K) \
	SHA1_CALC1( 0, f, K) \
	SHA1_CALC1( 1, f, K) \
	SHA1_CALC1( 2, f, K) \
	SHA1_CALC1( 3, f, K) \
	SHA1_CALC1( 4, f, K) \
	SHA1_CALC1( 5, f, K) \
	SHA1_CALC0(    f, W_06, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, 0, K) \
	SHA1_CALC0(    f, W_15, K) \
	W[16] = S(        W[ 2] ^ W[ 0], 1); \
	SHA1_CALC1(16, f, K) \
	W[17] = S(        W[ 3] ^ W[ 1], 1); \
	SHA1_CALC1(17, f, K) \
	W[18] = S(W_15 ^ W[ 4] ^ W[ 2], 1); \
	SHA1_CALC1(18, f, K) \
	W[19] = S(W[16] ^ W[ 5] ^ W[ 3], 1); \
	SHA1_CALC1(19, f, K)

#define SHA1_LOOP2(f, K) \
	W[20] = S(W[17] ^ W_06    ^ W[ 4], 1); \
	SHA1_CALC1(20, f, K) \
	W[21] = S(W[18]         ^ W[ 5], 1); \
	SHA1_CALC1(21, f, K) \
	W[22] = S(W[19]         ^ W_06   , 1); \
	SHA1_CALC1(22, f, K) \
	W[23] = S(W[20] ^ W_15, 1); \
	SHA1_CALC1(23, f, K) \
	W[24] = S(W[21] ^ W[16], 1); \
	SHA1_CALC1(24, f, K) \
	W[25] = S(W[22] ^ W[17], 1); \
	SHA1_CALC1(25, f, K) \
	W[26] = S(W[23] ^ W[18], 1); \
	SHA1_CALC1(26, f, K) \
	W[27] = S(W[24] ^ W[19], 1); \
	SHA1_CALC1(27, f, K) \
	W[28] = S(W[25] ^ W[20], 1); \
	SHA1_CALC1(28, f, K) \
	W[29] = S(W[26] ^ W[21] ^ W_15, 1); \
	SHA1_CALC1(29, f, K) \
	W[30] = S(W[27] ^ W[22] ^ W[16], 1); \
	SHA1_CALC1(30, f, K) \
	W[31] = S(W[28] ^ W[23] ^ W[17] ^ W_15, 1); \
	SHA1_CALC1(31, f, K) \
	SHA1_CALC2(32, f, K) \
	SHA1_CALC2(33, f, K) \
	SHA1_CALC2(34, f, K) \
	SHA1_CALC2(35, f, K) \
	SHA1_CALC2(36, f, K) \
	SHA1_CALC2(37, f, K) \
	SHA1_CALC2(38, f, K) \
	SHA1_CALC2(39, f, K)

#define SHA1_LOOP3(offset, f, K) SHA1_LOOP20(SHA1_CALC2, offset, f, K)

class CustomSha1 {
	static inline uint32_t S(uint32_t x, int s)
	{
#ifdef _MSC_VER
		return _rotl(x, s);
#else
		return (x << s) | (x >> (32 - s));
#endif
	}
#ifdef SHA1_USE_SIMD
	template<class T>
	static inline T S(T x, int s)
	{
		return (x << s) | (x >> (32 - s));
	}
#endif

	static inline uint32_t getBE(const char *p)
	{
#ifdef SHA1_DONT_USE_CYBOZULIB
		uint32_t a = (uint8_t)p[0];
		uint32_t b = (uint8_t)p[1];
		uint32_t c = (uint8_t)p[2];
		uint32_t d = (uint8_t)p[3];
		return (a << 24) | (b << 16) | (c << 8) | d;
#else
		return cybozu::Get32bitAsBE(p);
#endif
	}

	static inline void setBE(void *out, uint32_t x)
	{
#ifdef SHA1_DONT_USE_CYBOZULIB
		uint8_t *p = (uint8_t *)out;
		p[0] = uint8_t(x >> 24);
		p[1] = uint8_t(x >> 16);
		p[2] = uint8_t(x >> 8);
		p[3] = uint8_t(x);
#else
		cybozu::Set32bitAsBE(out, x);
#endif
	}
	static uint32_t f0(uint32_t b, uint32_t c, uint32_t d) { return (b & c) | ((~b) & d); }
#ifdef SHA1_USE_SIMD
	template<class T>
	static T f0(T b, T c, T d) { return (b & c) | andn(b, d); }
#endif
	template<class T>
	static T f1(T b, T c, T d) { return b ^ c ^ d; }
	template<class T>
	static T f2(T b, T c, T d) { return (b & c) | (b & d) | (c & d); }
	static inline int mod(int n) { return n & 15; }
	/*
		input : W[1..5]
	*/
	template<class T>
	static void digestOnce(T W[16])
	{
		T W_06 = 0x80000000;
		T W_07, W_08, W_09, W_10, W_11, W_12, W_13, W_14;
		T W_15 = 0xc0;

		/*
			input   = W[0..5]
			W[0]    = index
			W[1..5] = previous hash(input)
			W_06    = 0x80000000
			W_07..14= 0
			W_15    = 0xc0
			output : W[1..5]
		*/
		const T H0 = 0x67452301;
		const T H1 = 0xefcdab89;
		const T H2 = 0x98badcfe;
		const T H3 = 0x10325476;
		const T H4 = 0xc3d2e1f0;
		const T K0 = 0x5a827999;
		const T K1 = 0x6ed9eba1;
		const T K2 = 0x8f1bbcdc;
		const T K3 = 0xca62c1d6;
		T a = H0;
		T b = H1;
		T c = H2;
		T d = H3;
		T e = H4;

		SHA1_CALC0(f0, W[0], K0)
		SHA1_CALC0(f0, W[1], K0)
		SHA1_CALC0(f0, W[2], K0)
		SHA1_CALC0(f0, W[3], K0)
		SHA1_CALC0(f0, W[4], K0)
		SHA1_CALC0(f0, W[5], K0)
		SHA1_CALC0(f0, W_06, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, 0, K0)
		SHA1_CALC0(f0, W_15, K0)
		W[0] = S(              W[2] ^ W[0], 1); SHA1_CALC0(f0, W[0], K0)
		W[1] = S(              W[3] ^ W[1], 1); SHA1_CALC0(f0, W[1], K0)
		W[2] = S(W_15        ^ W[4] ^ W[2], 1); SHA1_CALC0(f0, W[2], K0)
		W[3] = S(W[0]        ^ W[5] ^ W[3], 1); SHA1_CALC0(f0, W[3], K0)
		W[4] = S(W[1]        ^ W_06 ^ W[4], 1); SHA1_CALC0(f1, W[4], K1)
		W[5] = S(W[2]               ^ W[5], 1); SHA1_CALC0(f1, W[5], K1)
		W_06 = S(W[3]               ^ W_06, 1); SHA1_CALC0(f1, W_06, K1)
		W_07 = S(W[4] ^ W_15              , 1); SHA1_CALC0(f1, W_07, K1)
		W_08 = S(W[5] ^ W[0]              , 1); SHA1_CALC0(f1, W_08, K1)
		W_09 = S(W_06 ^ W[1]              , 1); SHA1_CALC0(f1, W_09, K1)
		W_10 = S(W_07 ^ W[2]              , 1); SHA1_CALC0(f1, W_10, K1)
		W_11 = S(W_08 ^ W[3]              , 1); SHA1_CALC0(f1, W_11, K1)
		W_12 = S(W_09 ^ W[4]              , 1); SHA1_CALC0(f1, W_12, K1)
		W_13 = S(W_10 ^ W[5] ^ W_15       , 1); SHA1_CALC0(f1, W_13, K1)
		W_14 = S(W_11 ^ W_06 ^ W[0]       , 1); SHA1_CALC0(f1, W_14, K1)
		W_15 = S(W_12 ^ W_07 ^ W[1] ^ W_15, 1); SHA1_CALC0(f1, W_15, K1)
		W[0] = S(W_13 ^ W_08 ^ W[2] ^ W[0], 1); SHA1_CALC0(f1, W[0], K1)
		W[1] = S(W_14 ^ W_09 ^ W[3] ^ W[1], 1); SHA1_CALC0(f1, W[1], K1)
		W[2] = S(W_15 ^ W_10 ^ W[4] ^ W[2], 1); SHA1_CALC0(f1, W[2], K1)
		W[3] = S(W[0] ^ W_11 ^ W[5] ^ W[3], 1); SHA1_CALC0(f1, W[3], K1)
		W[4] = S(W[1] ^ W_12 ^ W_06 ^ W[4], 1); SHA1_CALC0(f1, W[4], K1)
		W[5] = S(W[2] ^ W_13 ^ W_07 ^ W[5], 1); SHA1_CALC0(f1, W[5], K1)
		W_06 = S(W[3] ^ W_14 ^ W_08 ^ W_06, 1); SHA1_CALC0(f1, W_06, K1)
		W_07 = S(W[4] ^ W_15 ^ W_09 ^ W_07, 1); SHA1_CALC0(f1, W_07, K1)
		W_08 = S(W[5] ^ W[0] ^ W_10 ^ W_08, 1); SHA1_CALC0(f2, W_08, K2)
		W_09 = S(W_06 ^ W[1] ^ W_11 ^ W_09, 1); SHA1_CALC0(f2, W_09, K2)
		W_10 = S(W_07 ^ W[2] ^ W_12 ^ W_10, 1); SHA1_CALC0(f2, W_10, K2)
		W_11 = S(W_08 ^ W[3] ^ W_13 ^ W_11, 1); SHA1_CALC0(f2, W_11, K2)
		W_12 = S(W_09 ^ W[4] ^ W_14 ^ W_12, 1); SHA1_CALC0(f2, W_12, K2)
		W_13 = S(W_10 ^ W[5] ^ W_15 ^ W_13, 1); SHA1_CALC0(f2, W_13, K2)
		W_14 = S(W_11 ^ W_06 ^ W[0] ^ W_14, 1); SHA1_CALC0(f2, W_14, K2)
		W_15 = S(W_12 ^ W_07 ^ W[1] ^ W_15, 1); SHA1_CALC0(f2, W_15, K2)
		W[0] = S(W_13 ^ W_08 ^ W[2] ^ W[0], 1); SHA1_CALC0(f2, W[0], K2)
		W[1] = S(W_14 ^ W_09 ^ W[3] ^ W[1], 1); SHA1_CALC0(f2, W[1], K2)
		W[2] = S(W_15 ^ W_10 ^ W[4] ^ W[2], 1); SHA1_CALC0(f2, W[2], K2)
		W[3] = S(W[0] ^ W_11 ^ W[5] ^ W[3], 1); SHA1_CALC0(f2, W[3], K2)
		W[4] = S(W[1] ^ W_12 ^ W_06 ^ W[4], 1); SHA1_CALC0(f2, W[4], K2)
		W[5] = S(W[2] ^ W_13 ^ W_07 ^ W[5], 1); SHA1_CALC0(f2, W[5], K2)
		W_06 = S(W[3] ^ W_14 ^ W_08 ^ W_06, 1); SHA1_CALC0(f2, W_06, K2)
		W_07 = S(W[4] ^ W_15 ^ W_09 ^ W_07, 1); SHA1_CALC0(f2, W_07, K2)
		W_08 = S(W[5] ^ W[0] ^ W_10 ^ W_08, 1); SHA1_CALC0(f2, W_08, K2)
		W_09 = S(W_06 ^ W[1] ^ W_11 ^ W_09, 1); SHA1_CALC0(f2, W_09, K2)
		W_10 = S(W_07 ^ W[2] ^ W_12 ^ W_10, 1); SHA1_CALC0(f2, W_10, K2)
		W_11 = S(W_08 ^ W[3] ^ W_13 ^ W_11, 1); SHA1_CALC0(f2, W_11, K2)
		W_12 = S(W_09 ^ W[4] ^ W_14 ^ W_12, 1); SHA1_CALC0(f1, W_12, K3)
		W_13 = S(W_10 ^ W[5] ^ W_15 ^ W_13, 1); SHA1_CALC0(f1, W_13, K3)
		W_14 = S(W_11 ^ W_06 ^ W[0] ^ W_14, 1); SHA1_CALC0(f1, W_14, K3)
		W_15 = S(W_12 ^ W_07 ^ W[1] ^ W_15, 1); SHA1_CALC0(f1, W_15, K3)
		W[0] = S(W_13 ^ W_08 ^ W[2] ^ W[0], 1); SHA1_CALC0(f1, W[0], K3)
		W[1] = S(W_14 ^ W_09 ^ W[3] ^ W[1], 1); SHA1_CALC0(f1, W[1], K3)
		W[2] = S(W_15 ^ W_10 ^ W[4] ^ W[2], 1); SHA1_CALC0(f1, W[2], K3)
		W[3] = S(W[0] ^ W_11 ^ W[5] ^ W[3], 1); SHA1_CALC0(f1, W[3], K3)
		W[4] = S(W[1] ^ W_12 ^ W_06 ^ W[4], 1); SHA1_CALC0(f1, W[4], K3)
		W[5] = S(W[2] ^ W_13 ^ W_07 ^ W[5], 1); SHA1_CALC0(f1, W[5], K3)
		W_06 = S(W[3] ^ W_14 ^ W_08 ^ W_06, 1); SHA1_CALC0(f1, W_06, K3)
		W_07 = S(W[4] ^ W_15 ^ W_09 ^ W_07, 1); SHA1_CALC0(f1, W_07, K3)
		W_08 = S(W[5] ^ W[0] ^ W_10 ^ W_08, 1); SHA1_CALC0(f1, W_08, K3)
		W_09 = S(W_06 ^ W[1] ^ W_11 ^ W_09, 1); SHA1_CALC0(f1, W_09, K3)
		W_10 = S(W_07 ^ W[2] ^ W_12 ^ W_10, 1); SHA1_CALC0(f1, W_10, K3)
		W_11 = S(W_08 ^ W[3] ^ W_13 ^ W_11, 1); SHA1_CALC0(f1, W_11, K3)
		W_12 = S(W_09 ^ W[4] ^ W_14 ^ W_12, 1); SHA1_CALC0(f1, W_12, K3)
		W_13 = S(W_10 ^ W[5] ^ W_15 ^ W_13, 1); SHA1_CALC0(f1, W_13, K3)
		W_14 = S(W_11 ^ W_06 ^ W[0] ^ W_14, 1); SHA1_CALC0(f1, W_14, K3)
		W_15 = S(W_12 ^ W_07 ^ W[1] ^ W_15, 1); SHA1_CALC0(f1, W_15, K3)

		W[1] = a + H0;
		W[2] = b + H1;
		W[3] = c + H2;
		W[4] = d + H3;
		W[5] = e + H4;
	}
public:
	/*
		input  h[0..19]
		output h[0..19]
	*/
	static inline void digest(char *h, int spinCount)
	{
		uint32_t W[80];
		for (int i = 1; i < 6; i++) {
			W[i] = getBE(&h[(i - 1) * 4]);
		}
		for (int i = 0; i < spinCount; i++) {
			// W[0]
			setBE(W, i);
			digestOnce<uint32_t>(W);
		}
		for (size_t i = 1; i < 6; i++) {
			setBE(&h[(i - 1) * 4], W[i]);
		}
	}
#ifdef SHA1_USE_SIMD
	/*
		in/out out[n]
		assume out[i].size() == 20
	*/
	template<int n>
	static inline void digestX(std::string out[n], int spinCount)
	{
		Uint32Vec W[80];
		for (int i = 1; i < 6; i++) {
			const int pos = (i - 1) * 4;
#ifdef UINT32VEC_USE_AVX2
			W[i].set(getBE(&out[7][pos]), getBE(&out[6][pos]), getBE(&out[5][pos]), getBE(&out[4][pos]), getBE(&out[3][pos]), getBE(&out[2][pos]), getBE(&out[1][pos]), getBE(&out[0][pos]));
#else
			W[i].set(getBE(&out[3][pos]), getBE(&out[2][pos]), getBE(&out[1][pos]), getBE(&out[0][pos]));
#endif
		}
		for (int i = 0; i < spinCount; i++) {
			W[0] = cybozu::byteSwap(uint32_t(i));
			digestOnce<Uint32Vec >(W);
		}
		for (size_t i = 1; i < 6; i++) {
			for (int j = 0; j < n; j++) {
				uint32_t x = getBE((const char*)&W[i] + j * 4);
				memcpy(&out[j][(i - 1) * 4], &x, sizeof(x));
			}
		}
	}
#endif
};
