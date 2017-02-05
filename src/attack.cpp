/**
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
#include "attack.hpp"
#include "make_dataspace.hpp"

struct Option {
	typedef std::vector<std::string> StrVec;
	std::string encFile;
	std::string charSet;
	StrVec passSet;
	size_t threadNum;
	size_t passLen;
	void trim(std::string& str) const
	{
		const size_t size = str.size();
		if (size == 0) return;
		const char c = str[size - 1];
		if (c == '\n' || c == '\r') str.resize(size - 1);
	}
	void toUtf8(std::string& str) const
	{
		const size_t size = str.size();
		std::string out(size * 2, '\0');
		for (size_t i = 0; i < size; i++) {
			out[i * 2] = str[i];
		}
		str.swap(out);
	}
	static inline bool lessByLength(const std::string& rhs, const std::string& lhs)
	{
		if (rhs.size() < lhs.size()) return true;
		if (rhs.size() > lhs.size()) return false;
		return rhs < lhs;
	}
	Option(int argc, const char *const argv[])
	{
		cybozu::Option opt;
		std::string charSetFile;
		std::string dicFile;
		bool debug;
		opt.appendOpt(&charSetFile, "", "cf", "char set file");
		opt.appendOpt(&threadNum, 4, "t", "thread num");
		opt.appendOpt(&dicFile, "", "d", "dictionary file");
		opt.appendOpt(&passLen, 0, "l", "length of pass");
		opt.appendBoolOpt(&debug, "v", "verbose message");
		opt.appendHelp("h");
		opt.appendParam(&encFile, "encrypted file");
		if (!opt.parse(argc, argv)) {
			opt.usage();
			exit(1);
		}
		if (debug) ms::setDebug(2);
		if (charSetFile.empty()) {
			charSet = "abcdefghijklmnopqrstuvwxyz"
			          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					  "0123456789-_";
		} else {
			std::ifstream ifs(charSetFile.c_str(), std::ios::binary);
			if (!std::getline(ifs, charSet)) {
				fprintf(stderr, "can't read char set file [%s]\n", charSetFile.c_str());
				exit(1);
			}
			trim(charSet);
		}
		if (!dicFile.empty()) {
			std::ifstream ifs(dicFile.c_str(), std::ios::binary);
			std::string line;
			while (std::getline(ifs, line)) {
				trim(line);
				toUtf8(line);
				passSet.push_back(line);
			}
			if (passSet.empty()) {
				fprintf(stderr, "can't read dicFile [%s]\n", dicFile.c_str());
				exit(1);
			}
			std::sort(passSet.begin(), passSet.end(), &lessByLength);
		}
		if (ms::isDebug()) {
			opt.put();
		}
	}
};

int main(int argc, char *argv[])
	try
{
#ifdef SHA1_USE_SIMD
	printf("use simd x %d\n", Uint32Vec::size);
#endif
	const Option opt(argc, argv);
	cybozu::Mmap m(opt.encFile);
	const char *data = m.get();
	if (m.size() > 0xffffffff) {
		throw cybozu::Exception("ms:encode:m.size") << m.size();
	}
	const uint32_t dataSize = static_cast<uint32_t>(m.size());
	const ms::Format format = ms::DetectFormat(data, dataSize);

	if (format == ms::fZip) {
		printf("already decrypted\n");
		return 2;
	}
	if (!opt.passSet.empty()) {
		ms::Attack attack(data, dataSize, opt.threadNum, opt.passSet);
	}
	if (opt.passLen) {
		ms::Attack attack(data, dataSize, opt.passLen);
	}
} catch (std::exception& e) {
	printf("exception:%s\n", e.what());
	return 1;
}
