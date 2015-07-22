#pragma once
/**
	@file
	@brief MS Office password atack
	Copyright (C) 2013 Cybozu Labs, Inc., all rights reserved.
*/
#include <fstream>
#include <assert.h>
#include <math.h>
#include <cybozu/mmap.hpp>
#include <cybozu/minixml.hpp>
#include <cybozu/time.hpp>
#include <cybozu/atoi.hpp>
#include <cybozu/crypto.hpp>
#include <cybozu/random_generator.hpp>
#include <cybozu/parallel.hpp>
#include <cybozu/atomic.hpp>
#include "cfb.hpp"
#include "crypto_util.hpp"
#include "decode.hpp"

namespace ms {

inline void setPass(std::string& pass, size_t idx, size_t passLen, const std::string& passCharTbl)
{
	pass.resize(passLen * 2);
	const size_t passCharNum = passCharTbl.size();
	for (size_t i = 0; i < passCharNum; i++) {
		size_t q = idx / passCharNum;
		size_t r = idx % passCharNum;
		pass[(passLen - 1 - i) * 2] = passCharTbl[r];
		pass[(passLen - 1 - i) * 2 + 1] = 0;
		idx = q;
	}
}

inline void putPass(const std::string& pass)
{
	for (size_t i = 0; i < pass.size(); i += 2) {
		putchar(pass[i]);
	}
}

struct Attack {
	std::string correctPass;
	size_t threadNum;
	struct PassSetData {
		const EncryptionInfo& info;
		const std::vector<std::string>& passSet;
		size_t offset;
		size_t threadNum;
		std::string& correctPass;
		int quit;
		uint64_t checkNum;
		PassSetData(const EncryptionInfo& info, const std::vector<std::string>& passSet, size_t offset, size_t threadNum, std::string& correctPass)
			: info(info)
			, passSet(passSet)
			, offset(offset)
			, threadNum(threadNum)
			, correctPass(correctPass)
			, quit(0)
			, checkNum(0)
		{
		}
		/*
			break if return false
		*/
		bool operator()(size_t i, size_t /*threadIdx*/)
		{
			if (quit) return false;
#ifdef SHA1_USE_SIMD
			const int N = Uint32Vec::size;
			if (i % N) return true;
			const std::string *p = &passSet[i + offset];
			cybozu::AtomicAdd(&checkNum, uint64_t(N));
			int idx = verifyPasswordX<N>(info, p);
			bool found = idx >= 0;
			const std::string& pass = p[found ? idx : 0];
#else
			const std::string& pass = passSet[i + offset];
			cybozu::AtomicAdd(&checkNum, uint64_t(1));
			bool found = verifyPassword(info, pass);
#endif
			if (found) {
				correctPass = toUtf8(pass);
				printf("found `%s`\n", correctPass.c_str());
				cybozu::AtomicAdd(&quit, 1);
				return false;
			}
			return true;
		}
	private:
		PassSetData(const PassSetData&);
		void operator=(const PassSetData&);
	};
	static inline std::string toUtf8(const std::string& str)
	{
		const size_t size = str.size();
		assert((size % 2) == 0);
		std::string ret;
		ret.resize(size / 2);
		for (size_t i = 0; i < size / 2; i++) {
			ret[i] = str[i * 2];
		}
		return ret;
	}
	static inline bool verifyPassword(const EncryptionInfo& info, const std::string& pass)
	{
//printf("check %s\n", toUtf8(pass).c_str());
		const CipherParam& encryptedKey = info.encryptedKey;
		const std::string& iv = encryptedKey.saltValue;

		const std::string pwHash = hashPassword(encryptedKey.hashName, iv, pass, info.spinCount);
		const std::string skey1 = generateKey(encryptedKey, pwHash, ms::blkKey_VerifierHashInput);
		const std::string skey2 = generateKey(encryptedKey, pwHash, ms::blkKey_encryptedVerifierHashValue);

		const std::string verifierHashInput = cipher(encryptedKey.cipherName, info.encryptedVerifierHashInput, skey1, iv, cybozu::crypto::Cipher::Decoding);
		const std::string hashedVerifier = cybozu::crypto::Hash::digest(encryptedKey.hashName, verifierHashInput);
		const std::string verifierHash = cipher(encryptedKey.cipherName, info.encryptedVerifierHashValue, skey2, iv, cybozu::crypto::Cipher::Decoding).substr(0, hashedVerifier.size());

		return hashedVerifier == verifierHash;
	}
#ifdef SHA1_USE_SIMD
	/*
		return the index of pass[] if valid
		otherwise -1
	*/
	template<int n>
	static inline int verifyPasswordX(const EncryptionInfo& info, const std::string pass[n])
	{
		const CipherParam& encryptedKey = info.encryptedKey;
		const std::string& iv = encryptedKey.saltValue;

		std::string pwHash[n];
		sha1PasswordX<n>(pwHash, iv, pass, info.spinCount);

		for (int i = 0; i < n; i++) {
			const std::string skey1 = generateKey(encryptedKey, pwHash[i], ms::blkKey_VerifierHashInput);
			const std::string skey2 = generateKey(encryptedKey, pwHash[i], ms::blkKey_encryptedVerifierHashValue);

			const std::string verifierHashInput = cipher(encryptedKey.cipherName, info.encryptedVerifierHashInput, skey1, iv, cybozu::crypto::Cipher::Decoding);
			const std::string hashedVerifier = cybozu::crypto::Hash::digest(encryptedKey.hashName, verifierHashInput);
			const std::string verifierHash = cipher(encryptedKey.cipherName, info.encryptedVerifierHashValue, skey2, iv, cybozu::crypto::Cipher::Decoding).substr(0, hashedVerifier.size());

			if (hashedVerifier == verifierHash) return i;
		}
		return -1;
	}
#endif
	Attack(const char *data, uint32_t dataSize, size_t threadNum, const std::vector<std::string>& passSet)
		: threadNum(threadNum)
	{
		ms::cfb::CompoundFile cfb(data, dataSize);
		cfb.put();
		const EncryptionInfo info(GetContensByName(cfb, "EncryptionInfo")); // xml
		printf("spinCount=%d\n", info.spinCount);
#if 1
		const size_t sepNum = 100000;
		size_t offset = 0;
		size_t remain = passSet.size();
		size_t totalCheckNum = 0;
		const double begin = cybozu::GetCurrentTimeSec();
		while (remain > 0) {
			printf("offset=%d\n", (int)offset);
			PassSetData target(info, passSet, offset, threadNum, correctPass);
			const size_t targetNum = std::min(remain, sepNum);
			cybozu::parallel_for(target, targetNum, threadNum);
			totalCheckNum += target.checkNum;
			double elapsedTime = cybozu::GetCurrentTimeSec() - begin;

			printf("%.1f sec done %u (try : %.1f count/sec : sha1 %.1f M/sec)\n", elapsedTime, (uint32_t)totalCheckNum, totalCheckNum / elapsedTime, totalCheckNum * info.spinCount / elapsedTime * 1e-6);
			if (!correctPass.empty()) break;
			offset += targetNum;
			remain -= targetNum;
		}
#else
		for (size_t i = 0, n = passSet.size(); i < n; i++) {
			if (verifyPassword(info, passSet[i])) {
				correctPass = toUtf8(passSet[i]);
				printf("found `%s`\n", correctPass.c_str());
			}
		}
#endif
	}
	Attack(const char *data, uint32_t dataSize, size_t passLen)
		: threadNum(0)
	{
		ms::cfb::CompoundFile cfb(data, dataSize);
		cfb.put();
		const EncryptionInfo info(GetContensByName(cfb, "EncryptionInfo")); // xml
		printf("spinCount=%d\n", info.spinCount);
		const std::string passCharTbl = "0123456789";
		const size_t passCharNum = passCharTbl.size();
		const size_t n = (size_t)pow(passCharNum, (double)passLen);
		printf("pass charNum = %d, len = %d, n = %lld\n", (int)passCharNum, (int)passLen, (long long)n);
		std::string pass;
		pass.resize(passLen * 2);
		for (size_t idx = 0; idx < n; idx++) {
			setPass(pass, idx, passLen, passCharTbl);
			if ((idx % 100) == 0) printf("idx=%d\n", (int)idx);
			if (verifyPassword(info, pass)) {
				printf("found pass "); putPass(pass); putchar('\n');
				return;
			}
		}
		puts("not found");
	}
};


} // ms
