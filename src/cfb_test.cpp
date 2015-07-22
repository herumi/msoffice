#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <locale>
#include <fstream>
#include <cybozu/mmap.hpp>
#include "cfb.hpp"


void usage()
{
	fprintf(stderr, "cfb_test -f <inFile>\n");
	exit(1);
}

std::string toAscii(const cybozu::String16& str)
{
	std::string ret;
	if (cybozu::ConvertUtf16ToUtf8(&ret, str)) {
		return ret;
	}
	return "";
}

void run(const std::string& inFile)
{
	cybozu::Mmap m(inFile);
	if (m.size() > 0xffffffff) {
		fprintf(stderr, "too large file size %lld\n", m.size());
		exit(1);
	}
	const uint32_t mSize = static_cast<uint32_t>(m.size());
	ms::cfb::CompoundFile cfb(m.get(), mSize);
	cfb.put();
	const char nameTbl[][128] = {
		"EncryptedPackage",
		"EncryptionInfo",
	};
	const ms::cfb::DirectoryEntryVec& dirs = cfb.dirs;
	for (size_t i = 0; i < dirs.size(); i++) {
		const ms::cfb::DirectoryEntry& dir = dirs[i];
		for (size_t j = 0; j < CYBOZU_NUM_OF_ARRAY(nameTbl); j++) {
			std::string asc = toAscii(dir.directoryEntryName);
			if (asc == nameTbl[j]) {
printf("save %s\n", nameTbl[j]);
				std::ofstream ofs(nameTbl[j], std::ios::binary);
				ofs.write((const char*)&dir.data[0], (size_t)dir.streamSize);
			}
		}
	}
}

int main(int argc, char *argv[])
try
{
	std::locale::global(std::locale(""));

	argc--, argv++;
	std::string inFile;
	while (argc > 0) {
		if (argc > 1 && strcmp(*argv, "-f") == 0) {
			argc--, argv++;
			inFile = *argv;
		} else
		{
			usage();
		}
		argc--, argv++;
	}
	if (inFile.empty()) {
		usage();
	}
	run(inFile);
} catch (std::exception& e) {
	fprintf(stderr, "exception:%s\n", e.what());
	return 1;
}
