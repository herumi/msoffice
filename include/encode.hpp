#pragma once
/**
	@file
	@brief MS Office encryption encoder
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/
#include <cybozu/crypto.hpp>
#include <cybozu/mmap.hpp>
#include <cybozu/random_generator.hpp>
#include "crypto_util.hpp"
#include "cfb.hpp"
#include "make_dataspace.hpp"
#include "resource.hpp"

//#define SAME_KEY

namespace ms {

inline cybozu::RandomGenerator& GetRandGen()
{
	static cybozu::RandomGenerator rg;
	return rg;
}

inline void FillRand(std::string& str, size_t n)
{
	str.resize(n);
	GetRandGen().read(&str[0], static_cast<int>(n));
}

#if 0
inline void VerifyFormat(const char *data, uint32_t dataSize)
{
	if (dataSize < 2) throw cybozu::Exception("ms:VerifyFormat:too small") << dataSize;
	if (memcmp(data, "PK", 2) != 0) throw cybozu::Exception("ms:VerifyFormat:bad format");
}
#endif

/*
	encryptedPackage = [uint64_t:encData]
*/
inline void MakeEncryptedPackage(std::string& encryptedPackage, const std::string& encData)
{
	encryptedPackage.reserve(encData.size() + 8);
	encryptedPackage.resize(8);
	cybozu::Set64bitAsLE(&encryptedPackage[0], encData.size());
	encryptedPackage += encData;
}

/*
	[MS-OFFCRYPTO] 2.3.4.14
*/
inline void GenerateIntegrityParameter(
	std::string& encryptedHmacKey,
	std::string& encryptedHmacValue,
	const std::string& encryptedPackage,
	const CipherParam& keyData,
	const std::string& secretKey,
	const std::string& saltValue)
{
	std::string salt;
	FillRand(salt, keyData.hashSize);
#ifdef SAME_KEY
	salt = fromHex("C9FACA5436849906B600DE95E155B47A01ABEDD0");
#endif
	const std::string iv1 = generateIv(keyData, ms::blkKey_dataIntegrity1, saltValue);
	const std::string iv2 = generateIv(keyData, ms::blkKey_dataIntegrity2, saltValue);
	encryptedHmacKey = cipher(keyData.cipherName, salt, secretKey, iv1, cybozu::crypto::Cipher::Encoding);
	cybozu::crypto::Hmac hmac(keyData.hashName);
	std::string ret = hmac.eval(salt, encryptedPackage);
	encryptedHmacValue = cipher(keyData.cipherName, ret, secretKey, iv2, cybozu::crypto::Cipher::Encoding);
}

inline void EncContent(std::string& encryptedPackage, const std::string& org, const CipherParam& param, const std::string& key, const std::string& salt)
{
	uint64_t orgSize = org.size();
	const size_t blockSize = 4096;
	std::string data = org;
	data.resize(RoundUp(data.size(), size_t(16)));
#ifdef SAME_KEY
	data[data.size() - 2] = 0x4b; // QQQ remove this
	data[data.size() - 1] = 0x6a;
#endif
	encryptedPackage.reserve(data.size() + 8);
	encryptedPackage.resize(8);
	cybozu::Set64bitAsLE(&encryptedPackage[0], orgSize);

	const size_t n = (data.size() + blockSize - 1) / blockSize;
	for (size_t i = 0; i < n; i++) {
		const size_t len = (i < n - 1) ? blockSize : (data.size() - blockSize * i);
		std::string blockKey(4, 0);
		cybozu::Set32bitAsLE(&blockKey[0], static_cast<uint32_t>(i));
		const std::string iv = generateKey(param, salt, blockKey);
		encryptedPackage.append(cipher(param.cipherName, data.c_str() + i * blockSize, len, key, iv, cybozu::crypto::Cipher::Encoding));
	}
}

/*
‚±‚ê‚Ì‹t‡‚ÅˆÃ†‰»
fix parameter : c1(blkKey_VerifierHashInput)
                c2(blkKey_encryptedVerifierHashValue)
                c3(blkKey_encryptedKeyValue)

input : pass, spinCount
output: iv,
        verifierHashInput,
        encryptedVerifierHashValue,
        encryptedKeyValue

iv(encryptedKey.saltValue)‚ðƒ‰ƒ“ƒ_ƒ€¶¬

pwHash = hashPassword(iv, pass, spinCount)
skey1 = generateKey(pwHash, c1)
skey2 = generateKey(pwHash, c2)

verifierHashInput‚ðƒ‰ƒ“ƒ_ƒ€¶¬
encryptedVerifierHashInput = Enc(verifierHashInput, skey1, iv)
hashedVerifier = H(verifierHashInput)

encryptedVerifierHashValue = Enc(verifierHash, skey2, iv)

skey3 = generateKey(pwHash, c3)
secretKey‚ðƒ‰ƒ“ƒ_ƒ€¶¬
encryptedKeyValue = Enc(secretKey, skey3, iv)
*/

/*
	encode data by pass with cipherName, hashName, spinCount
	output encData and info
*/

inline bool encode_in(
	std::string& encryptedPackage,
	EncryptionInfo& info,
	const std::string& data,
	cybozu::crypto::Cipher::Name cipherName,
	cybozu::crypto::Hash::Name hashName,
	int spinCount,
	const std::string& pass,
	const std::string& masterKey)
{
	if (spinCount > 10000000) throw cybozu::Exception("ms:encode_in:too large spinCount") << spinCount;
	CipherParam& keyData = info.keyData;
	CipherParam& encryptedKey = info.encryptedKey;

	keyData.setByName(cipherName, hashName);
	encryptedKey.setByName(cipherName, hashName);
	info.spinCount = spinCount;

	std::string& iv = encryptedKey.saltValue;
	FillRand(iv, encryptedKey.saltSize);
#ifdef SAME_KEY
	puts("QQQ defined SAME_KEY QQQ");
	iv = fromHex("F4994F9B2DCD5E0E84BC6386D4523D2C");
#endif
	const std::string pwHash = hashPassword(encryptedKey.hashName, iv, pass, spinCount);

	const std::string skey1 = generateKey(encryptedKey, pwHash, blkKey_VerifierHashInput);
	const std::string skey2 = generateKey(encryptedKey, pwHash, blkKey_encryptedVerifierHashValue);
	const std::string skey3 = generateKey(encryptedKey, pwHash, blkKey_encryptedKeyValue);

	std::string verifierHashInput;
	FillRand(verifierHashInput, encryptedKey.saltSize);
#ifdef SAME_KEY
	verifierHashInput = fromHex("FEDAECD950F9E82C47CADA29B7837C6D");
#endif

	verifierHashInput.resize(RoundUp(verifierHashInput.size(), encryptedKey.blockSize));

	info.encryptedVerifierHashInput = cipher(encryptedKey.cipherName, verifierHashInput, skey1, iv, cybozu::crypto::Cipher::Encoding);
	std::string hashedVerifier = cybozu::crypto::Hash::digest(encryptedKey.hashName, verifierHashInput);
	hashedVerifier.resize(RoundUp(hashedVerifier.size(), encryptedKey.blockSize));

	info.encryptedVerifierHashValue = cipher(encryptedKey.cipherName, hashedVerifier, skey2, iv, cybozu::crypto::Cipher::Encoding);

	std::string secretKey;
	FillRand(secretKey, encryptedKey.saltSize);
#ifdef SAME_KEY
	secretKey = fromHex("BF44FBB51BE1E88BF130156E117E7900");
#endif
	if (!masterKey.empty()) {
		secretKey = masterKey;
	}
	normalizeKey(secretKey, encryptedKey.keyBits / 8);

	info.encryptedKeyValue = cipher(encryptedKey.cipherName, secretKey, skey3, iv, cybozu::crypto::Cipher::Encoding);

	FillRand(keyData.saltValue, keyData.saltSize);
#ifdef SAME_KEY
	keyData.saltValue = fromHex("C49AAAEE99004C6B017EE5CD11B86729");
#endif

	EncContent(encryptedPackage, data, encryptedKey, secretKey, keyData.saltValue);

	GenerateIntegrityParameter(info.encryptedHmacKey, info.encryptedHmacValue, encryptedPackage, keyData, secretKey, keyData.saltValue);
	return true;
}

template<class String>
bool encode(const char *data, uint32_t dataSize, const String& outFile, const std::string& pass, bool isOffice2013, const std::string& masterKey, int spinCount)
{
	std::string encryptedPackage;
	ms::EncryptionInfo info;
	const cybozu::crypto::Cipher::Name cipherName = isOffice2013 ? cybozu::crypto::Cipher::N_AES256_CBC : cybozu::crypto::Cipher::N_AES128_CBC;
	const cybozu::crypto::Hash::Name hashName = isOffice2013 ? cybozu::crypto::Hash::N_SHA512 : cybozu::crypto::Hash::N_SHA1;
	encode_in(encryptedPackage, info, std::string(data, dataSize), cipherName, hashName, spinCount, pass, masterKey);
	const std::string encryptionInfoStr = info.addHeader(info.toXml(isOffice2013));
	dprintf("encryptionInfoStr size=%d\n", (int)encryptionInfoStr.size());
	ms::cfb::CompoundFile cfb;
	ms::makeDataSpace(cfb.dirs, encryptedPackage, encryptionInfoStr);
	std::string outData;
	makeLayout(outData, cfb);
	{
		cybozu::File out;
		out.openW(outFile);
		out.write(outData.c_str(), outData.size());
	}
	return true;
}

} // ms
