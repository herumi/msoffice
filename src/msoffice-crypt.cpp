/**
	@file
	@brief MS Office encryption encoder/decoder
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <locale>
#include <fstream>
#include <cybozu/mmap.hpp>
#include <cybozu/file.hpp>
#include <cybozu/atoi.hpp>
#include <cybozu/option.hpp>
#include "cfb.hpp"
#include "decode.hpp"
#include "encode.hpp"
#include "make_dataspace.hpp"
#ifdef _MSC_VER
#include <cybozu/string.hpp>
#endif

inline cybozu::String16 fromUniHex(const std::string& str)
{
	const size_t n = str.size();
	if ((n % 5) != 0) throw cybozu::Exception("f8romUniHex:bad str") << str;
	cybozu::String16 ret;
	for (size_t i = 0; i < n; i += 5) {
		if (str[i] != 'u') throw cybozu::Exception("fromUniHex:bad format") << str;
		cybozu::Char16 c = static_cast<uint16_t>(cybozu::hextoi(&str[i + 1], 4));
		ret += c;
	}
	return ret;
}

const char denySuffixTbl[][8] = {
	"xls",
	"xlt",
	"xla",

	"ppt",
	"pot",
	"pps",
	"ppa",

	"doc",
	"dot",
};

bool denySuffix(const std::string& suf)
{
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(denySuffixTbl); i++) {
		if (suf == denySuffixTbl[i]) return true;
	}
	return false;
}

#ifdef _MSC_VER
int wmain(int argc, wchar_t *wargv[])
#else
int main(int argc, char *argv[])
#endif
	try
{
#ifdef _MSC_VER
	std::vector<std::string> strVec;
	for (size_t i = 0; i < argc; i++) {
		const std::wstring s = __wargv[i];
		std::string utf8;
		cybozu::ConvertUtf16ToUtf8(&utf8, s);
		strVec.push_back(utf8);
	}
	std::vector<char*> ptrVec(argc);
	for (size_t i = 0; i < argc; i++) {
		ptrVec[i] = &strVec[i][0];
	}
	char **argv = &ptrVec[0];
#endif
//	std::locale::global(std::locale(""));

	std::string inFileA;
	std::string outFileA;
	std::string keyFile;
	std::string pstr, ph8str, ph16str;
	cybozu::String16 wpass;
	std::string secretKeyHex;
	bool doEncode = false, doDecode = false, doView = false;
	int encMode = 0;
	int spinCount = 0;
	bool putEncryptionInfo = false;
	bool debug2 = false;
	bool debug3 = false;
	bool putSecretKey = false;

	cybozu::Option opt;
	opt.appendOpt(&pstr, "", "p", "password in only ascii");
	opt.appendOpt(&encMode, 0, "encMode", "0:use AES128(default), 1: use AES256 for encoding");
	opt.appendOpt(&ph8str, "", "ph8", "password in utf8 hex. ex. 68656C6C6F for 'hello'");
	opt.appendOpt(&ph16str, "", "ph16", "password in utf16 hex. ex. u3042u3044u3046 for 'aiu' in hiragana");
	opt.appendOpt(&secretKeyHex, "", "k", "(experimental) secret key in hex. ex. 0123456789ABCDEF0123456789ABCDEF");
	opt.appendOpt(&keyFile, "", "by", "(experimental) extract secret key from this file");
	opt.appendBoolOpt(&doEncode, "e", "encode");
	opt.appendBoolOpt(&doDecode, "d", "decode");
	opt.appendOpt(&spinCount, 100000, "c", "spin count");
	opt.appendBoolOpt(&putSecretKey, "psk", "print secret key");
	opt.appendBoolOpt(&putEncryptionInfo, "info", "print EncryptionInfo info");
	opt.appendBoolOpt(&debug2, "v", "print debug info");
	opt.appendBoolOpt(&debug3, "vv", "print debug info and save binary data");
	opt.appendHelp("h");
	opt.appendParam(&inFileA, "input");
	opt.appendParamOpt(&outFileA, "", "output");

	if (!opt.parse(argc, argv)) {
		opt.usage();
		return 1;
	}
	ms::setDebug(debug3 ? 3 : debug2 ? 2 : putEncryptionInfo ? 1 : 0);
	if (!pstr.empty()) {
		wpass = cybozu::ToUtf16(pstr);
	}
	if (putSecretKey) {
		ms::putSecretKeyInstance() = true;
	}
	if (!ph8str.empty()) {
		wpass = cybozu::ToUtf16(ms::fromHex(ph8str));
	}
	if (!ph16str.empty()) {
		wpass = fromUniHex(ph16str);
	}
	std::string secretKey;
	if (!secretKeyHex.empty()) {
		secretKey = ms::fromHex(secretKeyHex, true);
	}
	if (!doEncode && !doDecode) {
		doDecode = true;
		doView = true;
	}
	if (keyFile.empty() && secretKey.empty() && wpass.empty()) {
		puts("specify -p password");
		opt.usage();
	}

	if (outFileA.empty()) {
		std::string suf;
		const std::string base = cybozu::GetBaseName(inFileA, &suf);
		if (denySuffix(suf)) {
			printf("bad input file [%s]. does not support %s\n", inFileA.c_str(), suf.c_str());
			return 1;
		}
		outFileA = base +(doEncode ? "_e." : "_d.") + suf;
	}
	printf("inFile=%s, outFile=%s, mode=%s, encMode=%d\n", inFileA.c_str(), outFileA.c_str(), doEncode ? "enc" : doView ? "view" : "dec", encMode);
	if (!secretKey.empty()) {
		printf("set secretKey = "); ms::dump(secretKey, false);
	}
	if (!keyFile.empty()) {
		printf("keyFile = %s\n", keyFile.c_str());
	}
	const std::string passData = ms::Char16toChar8(wpass);

#ifdef _MSC_VER
	std::wstring inFile;
	std::wstring outFile;
	cybozu::ConvertUtf8ToUtf16(&inFile, inFileA);
	cybozu::ConvertUtf8ToUtf16(&outFile, outFileA);
#else
	const std::string inFile = inFileA;
	const std::string outFile = outFileA;
#endif
	cybozu::Mmap m(inFile);
	const char *data = m.get();
	if (m.size() > 0xffffffff) {
		throw cybozu::Exception("ms:encode:m.size") << m.size();
	}
	const uint32_t dataSize = static_cast<uint32_t>(m.size());
	const ms::Format format = ms::DetectFormat(data, dataSize);


	if (doEncode) {
		if (format == ms::fCfb) {
			printf("already encrypted\n");
			return 2;
		}
		bool isOffice2013 = encMode == 1;
		ms::encode(data, dataSize, outFile, passData, isOffice2013, secretKey, spinCount);
	} else {
		if (format == ms::fZip) {
			printf("already decrypted\n");
			return 2;
		}
		if (!keyFile.empty()) {
			secretKey = ms::getSecretKey(keyFile, passData);
			printf("get secretKey = "); ms::dump(secretKey, false);
		}
		bool ok = ms::decode(data, dataSize, outFile, passData, secretKey, doView);
		if (!ok) {
			printf("bad password\n");
			return 3;
		}
	}
} catch (std::exception& e) {
	printf("exception:%s\n", e.what());
	return 1;
}
