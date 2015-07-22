#include <stdio.h>
#include <stdlib.h>
#include <cybozu/sha1.hpp>
#include "aes.hpp"
#include <cybozu/base64.hpp>
#include <sstream>
#include <fstream>
#include <cybozu/mmap.hpp>
#include <cybozu/minixml.hpp>
#include <cybozu/atoi.hpp>

/*
<keyData
  saltSize="16"
  blockSize="16"
  keyBits="128"
  hashSize="20"
  cipherAlgorithm="AES"
  cipherChaining="ChainingModeCBC"
  hashAlgorithm="SHA1"
  saltValue="xJqq7pkATGsBfuXNEbhnKQ=="
/>
<dataIntegrity
  encryptedHmacKey="rxCSo+nqBf18m0mctP82tZbBRRaC5h7DM1q7q/bKPrU="
  encryptedHmacValue="yuBiZXxrtgU3WNRWahDolVZJ8PbwsI9w7Hksnu2UkvM="
/>
<keyEncryptors>
  <p:encryptedKey
    spinCount="100000"
    saltSize="16"
    blockSize="16"
    keyBits="128"
    hashSize="20"
    cipherAlgorithm="AES"
    cipherChaining="ChainingModeCBC"
    hashAlgorithm="SHA1"
    saltValue="9JlPmy3NXg6EvGOG1FI9LA=="
    encryptedVerifierHashInput="xpwBRhK5VWSrmM9kspOnwg=="
    encryptedVerifierHashValue="jpuVmPpsYgBZR1fvJ21E9BhRTtuIlbJsknEjEPz1wmI="
    encryptedKeyValue="cnNcvlFXRSEykrLjpWgYlw=="
  />
</keyEncryptor>
*/

void dump(const std::string& str)
{
	for (size_t i = 0; i < str.size(); i++) {
		printf("%02X:", (unsigned char)str[i]);
	}
	printf("\n");
}

std::string hex(const std::string& str)
{
	std::string ret;
	for (size_t i = 0; i < str.size(); i++) {
		char buf[64];
		CYBOZU_SNPRINTF(buf, sizeof(buf), "%02X", (unsigned char)str[i]);
		ret.append(buf, 2);
	}
	return ret;
}

inline std::string dec64(const std::string& str)
{
	cybozu::MemoryInputStream is(str);
	cybozu::StringOutputStream os;
	cybozu::DecodeFromBase64(os, is);
	return os.str_;
}

std::string hashPassword(const std::string& salt, const std::string& pass, int spinCount)
{
	cybozu::Sha1 s;
	s.update(salt);
	std::string h = s.digest(pass);
	for (int i = 0; i < spinCount; i++) {
		char iter[4];
		cybozu::Set32bitAsLE(iter, i);
		cybozu::Sha1 s;
		s.update(iter, sizeof(iter));
		h = s.digest(h);
	}
	return h;
}

std::string hash(const std::string& msg)
{
	cybozu::Sha1 s;
	return s.digest(msg);
}

std::string generateKey(const std::string& hash, const std::string& blockKey)
{
	cybozu::Sha1 s;
	s.update(hash);
	return s.digest(blockKey).substr(0, 16);
}

std::string generateIv(const std::string& salt, const std::string& blockKey)
{
	if (blockKey.empty()) {
		return salt.substr(0, 16);
	}
	return generateKey(salt, blockKey);
}

std::string cipher(const char *msg, size_t msgLen, const std::string& key, const std::string& iv)
{
	printf("msg len=%d\n", (int)msgLen);
	if ((msgLen % 16) != 0) {
		puts("only size= 16x");
		exit(1);
	}
	cybozu::Aes cipher;
	if (!cipher.setup(hex(key).c_str(), cybozu::Aes::DECODING, 128, hex(iv).c_str())) {
		fprintf(stderr, "can't setup\n");
		exit(1);
	}
	std::string ret;
	for (size_t i = 0; i < msgLen; i += 16) {
		char outBuf[256];
		int writeSize = cipher.update(&msg[i], 16, outBuf);
		if (writeSize < 0) {
			exit(1);
		}
		ret.append(outBuf, writeSize);
	}
	return ret;
}

std::string cipher(const std::string& msg, const std::string& key, const std::string& iv)
{
	return cipher(msg.c_str(), msg.size(), key, iv);
}

std::string decContent(const std::string& data, const std::string& key, const std::string& salt)
{
	std::string dec;
	dec.reserve(data.size());
	const size_t n = (data.size() + 4095) / 4096;
	for (size_t i = 0; i < n; i++) {
		const size_t len = (i < n - 1) ? 4096 : (data.size() % 4096);
		std::string blockKey(4, 0);
		cybozu::Set32bitAsLE(&blockKey[0], i);
		const std::string iv = generateKey(salt, blockKey);
		dec.append(cipher(data.c_str() + i * 4096, len, key, iv));
	}
	return dec;
}

int main()
	try
{
	cybozu::Mmap xmlFile("EncryptionInfo");
	cybozu::MiniXml xml(xmlFile.get() + 8, xmlFile.get() + xmlFile.size());

	const cybozu::minixml::Node *keyData = xml.get().getFirstTagByName("keyData");
	const std::string keyData_saltValue(dec64(keyData->attr["saltValue"]));

	const cybozu::minixml::Node *dataIntegrity = xml.get().getFirstTagByName("dataIntegrity");

	const std::string encryptedHmacKey(dec64(dataIntegrity->attr["encryptedHmacKey"]));
	const std::string encryptedHmacValue(dec64(dataIntegrity->attr["encryptedHmacValue"]));

	// keyEncryptors
	const cybozu::minixml::Node *p_encryptedKey = xml.get().getFirstTagByName("p:encryptedKey");
	const int spinCount = cybozu::atoi(p_encryptedKey->attr["spinCount"]);
	const std::string encryptedKey_saltValue(dec64(p_encryptedKey->attr["saltValue"]));
	const std::string encryptedVerifierHashInput(dec64(p_encryptedKey->attr["encryptedVerifierHashInput"]));
	const std::string encryptedVerifierHashValue(dec64(p_encryptedKey->attr["encryptedVerifierHashValue"]));
	const std::string encryptedKeyValue(dec64(p_encryptedKey->attr["encryptedKeyValue"]));

	std::string pass("t\x00""e\x00""s\x00""t\x00", 8);
	const std::string kV("\xfe" "\xa7" "\xd2" "\x76" "\x3b" "\x4b" "\x9e" "\x79", 8);
	const std::string kH("\xd7" "\xaa" "\x0f" "\x6d" "\x30" "\x61" "\x34" "\x4e", 8);
	const std::string kC("\x14" "\x6e" "\x0b" "\xe7" "\xab" "\xac" "\xd0" "\xd6", 8);

	puts("pass"); dump(pass);
	const std::string pwHash = hashPassword(encryptedKey_saltValue, pass, spinCount);

	const std::string iv = encryptedKey_saltValue;
	const std::string skey1 = generateKey(pwHash, kV);
	const std::string verifierHashInput = cipher(encryptedVerifierHashInput, skey1, iv);
	puts("verifierHashInput");
	dump(verifierHashInput);
	const std::string hashedVerifier = hash(verifierHashInput);
	puts("hashedVerifier"); dump(hashedVerifier);

	const std::string skey2 = generateKey(pwHash, kH);
	const std::string verifierHash = cipher(encryptedVerifierHashValue, skey2, iv).substr(0, hashedVerifier.size());
	puts("verifierHash"); dump(verifierHash);

	if (hashedVerifier != verifierHash) {
		puts("miss");
		return 1;
	}
	const std::string skey3 = generateKey(pwHash, kC);
	std::string secretKey = cipher(encryptedKeyValue, skey3, iv);
	puts("secretKey"); dump(secretKey);

	cybozu::Mmap m("EncryptedPackage");
	uint64_t fileSize = cybozu::Get64bitAsLE(m.get());
	printf("fileSize=%d\n", (int)fileSize);
	std::string encData(m.get() + 8, (int)m.size() - 8);
printf("encData.size=%d\n", (int)encData.size());

	// decode
	std::string decData = decContent(encData, secretKey, keyData_saltValue);
	{
		std::ofstream ofs("dec.pptx", std::ios::binary);
		ofs.write(decData.c_str(), (int)fileSize);
	}
	// integrity
	{
		const std::string b1("\x5f" "\xb2" "\xad" "\x01" "\x0c" "\xb9" "\xe1" "\xf6", 8);
		const std::string iv1 = generateIv(keyData_saltValue, b1);
		const std::string salt = cipher(encryptedHmacKey, secretKey, iv1).substr(0, 20);
		printf("salt=%s\n", hex(salt).c_str());
	}
	{
		const std::string b2("\xa0" "\x67" "\x7f" "\x02" "\xb2" "\x2c" "\x84" "\x33", 8);
		const std::string iv2 = generateIv(keyData_saltValue, b2);
		const std::string r2 = cipher(encryptedHmacValue, secretKey, iv2).substr(0, 20);
		printf("  r2=%s\n", hex(r2).c_str());
	}
} catch (cybozu::Exception& e) {
	printf("ERR %s\n", e.what());
}
