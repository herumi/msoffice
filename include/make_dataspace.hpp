#pragma once
/**
	Copyright (C) 2012 Cybozu Labs, Inc., all rights reserved.
*/

#include <cybozu/time.hpp>
#include "cfb.hpp"
#include "resource.hpp"

namespace ms {

enum DSposition {
	iRoot = 0,
	iEncryptionPackage, // 1
	iDataSpaces, // 2
	iVersion, // 3
	iDataSpaceMap, // 4
	iDataSpaceInfo, // 5
	iStongEncryptionDataSpace, // 6
	iTransformInfo, // 7
	iStrongEncryptionTransform, // 8
	iPrimary, // 9
	iEncryptionInfo, // 10
	dirNum
};

/*
	make Data Spaces
*/
inline void makeDataSpace(
	cfb::DirectoryEntryVec& dirs,
	const std::string& encryptedPackage,
	const std::string& encryptionInfoXml)
{
	using namespace cfb;
	cybozu::Time curTime;
	curTime.setCurrentTime();
	DirectoryEntry::FileTime fileTime;
	curTime.getFILETIME(fileTime.dwLowDateTime, fileTime.dwHighDateTime);

	/*

0:Root/ 1:EncryptionPackage(F)
      /10:EncryptionInfo(F)
      / 2:DataSpaces/3:Version(F)
                    /4:DataSpaceMap(F)
                    /5:DataSpaceInfo/6:StrongEncryptionDataSpace(F)
                    /7:Transformation/8:StrongEncryptionTransform/9:Primary(F)

  <10:B>
   Å^Å_
<2:R> <1:R>
---------------
  <4:B>
   Å^Å_
<3:B>  <5:B>
         Å_
          <7:R>
---------------
<6:B>
---------------
<8:B>
---------------
<9:B>

	*/

	const struct Info {
		const char *name;
		ObjectType objectType;
		ColorFlag colorFlag;
		uint32_t leftSiblingId;
		uint32_t rightSiblingId;
		uint32_t childId;
		std::string content;
	} infoTbl[] = {
		{ "Root Entry",                RootStorageObject, Red,   NOSTREAM, NOSTREAM, iEncryptionInfo, "" },
		{ "EncryptedPackage",          StreamObject,      Red,   NOSTREAM, NOSTREAM, NOSTREAM, encryptedPackage },
		{ "\x06""DataSpaces",          StorageObject,     Red,   NOSTREAM, NOSTREAM, iDataSpaceMap, "" },
		{ "Version",                   StreamObject,      Black, NOSTREAM, NOSTREAM, NOSTREAM, resource::Version },
		{ "DataSpaceMap",              StreamObject,      Black, iVersion, iDataSpaceInfo, NOSTREAM, resource::DataSpaceMap },
		{ "DataSpaceInfo",             StorageObject,     Black, NOSTREAM, iTransformInfo, iStongEncryptionDataSpace, "" },
		{ "StrongEncryptionDataSpace", StreamObject,      Black, NOSTREAM, NOSTREAM, NOSTREAM, resource::StrongEncryptionDataSpace },
		{ "TransformInfo",             StorageObject,     Red,   NOSTREAM, NOSTREAM, iStrongEncryptionTransform, "" },
		{ "StrongEncryptionTransform", StorageObject,     Black, NOSTREAM, NOSTREAM, iPrimary, "" },
		{ "\x06""Primary",             StreamObject,      Black, NOSTREAM, NOSTREAM, NOSTREAM, resource::Primary },
		{ "EncryptionInfo",            StreamObject,      Black, iDataSpaces, iEncryptionPackage, NOSTREAM, encryptionInfoXml },
	};
	dirs.resize(dirNum);
	const std::string clsid = "";
	const uint32_t stateBits = 0;
	for (size_t i = 0; i < dirs.size(); i++) {
		uint32_t startingSectorLocation = 0;
		const Info& info = infoTbl[i];
		dirs[i].set(
			cybozu::ToUtf16(info.name),
			info.objectType,
			info.colorFlag,
			info.leftSiblingId,
			info.rightSiblingId,
			info.childId,
			clsid,
			stateBits,
			fileTime,
			fileTime,
			startingSectorLocation,
			info.content);
	}
}

inline void makeDifat(char *data, uint32_t difatPos, uint32_t difatSectorNum, uint32_t fatSectorNum, uint32_t sectorSize)
{
	uint32_t v = ms::cfb::Header::firstNumDIFAT + difatSectorNum;
	for (uint32_t i = 0; i < difatSectorNum; i++) {
dprintf("\ndifatSectorNum i=%d\n", i);
		char *p = data + 512 + (difatPos + i) * sectorSize;
		for (uint32_t j = 0; j < sectorSize / 4 - 1; j++) {
			cybozu::Set32bitAsLE(p + j * 4, v);
			v++;
dprintf("%d ", v);
			if (v > difatSectorNum + fatSectorNum) {
				for (; j < sectorSize / 4 - 1; j++) {
					cybozu::Set32bitAsLE(p + j * 4, ms::cfb::FREESECT);
				}
dprintf("endof\n");
				cybozu::Set32bitAsLE(p + sectorSize - 4, ms::cfb::ENDOFCHAIN);
				return;
			}
		}
dprintf("next %d\n", difatPos + i + 1);
		cybozu::Set32bitAsLE(p + sectorSize - 4, difatPos + i + 1);
	}
}

inline void makeFat(char *data, const char *dataEnd, const cfb::UintVec& entry)
{
	uint32_t v = 0;
	for (size_t i = 0; i < entry.size(); i++) {
		const uint32_t n = entry[i];
		if (n <= cfb::MAXREGSECT) {
			for (uint32_t j = 1; j < n; j++) {
				v++;
				if (data == dataEnd) throw cybozu::Exception("ms:makeFat:err1") << v << i << j;
				cybozu::Set32bitAsLE(data, v);
				data += 4;
			}
			if (data == dataEnd) throw cybozu::Exception("ms:makeFat:err2") << v << i;
			cybozu::Set32bitAsLE(data, cfb::ENDOFCHAIN);
		} else {
			cybozu::Set32bitAsLE(data, n);
		}
		data += 4;
		v++;
	}
	while (data != dataEnd) {
		cybozu::Set32bitAsLE(data, cfb::FREESECT);
		data += 4;
	}
}

inline void detectSectorNum(uint32_t& difatSectorNum, uint32_t& fatSectorNum, uint32_t n, uint32_t sectorSize)
{
	const uint32_t numInFat = sectorSize / 4;
	difatSectorNum = 0;
	fatSectorNum = 0;
	int count = 0;
	for (;;) {
		uint32_t a = getBlockNum(difatSectorNum + fatSectorNum + n + 0, numInFat);
		uint32_t b = (a <= cfb::Header::firstNumDIFAT) ? 0 : getBlockNum(a - cfb::Header::firstNumDIFAT, numInFat - 1);
		if (b == difatSectorNum && a == fatSectorNum) return;
		difatSectorNum = b;
		fatSectorNum = a;
		count++;
		if (count == 10) cybozu::Exception("ms:detectSectorNum:max loop") << difatSectorNum << fatSectorNum << n;
	}
}

template<class T>
uint32_t getMiniFatSectorNum(T size)
{
	return getBlockNum(static_cast<uint32_t>(size), 64u);
}
/*
	@note DIFAT is empty for current version

	|Header|DIFAT0|DIFAT1|...|FAT0|FAT1|...|miniFAT|dir0|dir1|dir2|min0|min1|min2|min3|enc0|enc1|...

	FAT
	1       ; miniFAT
	2 3 4   ; directoryEntry([d0:d1:d2])
	5 6 7 8 ; miniFATdata([m0:m1:m2:m3])
	9 10... ; encryptedPackage([c0:c1:...])

	miniFAT(64byte x 8 x 4)
	0 1 ; Version
	2 3 ; DataSpaceMap
	4
	5 6 7 8
	9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
*/
inline void makeLayout(std::string& data, cfb::CompoundFile& cfb)
{
	cfb::Header& header = cfb.header;
	cfb::DirectoryEntryVec& dirs = cfb.dirs;
	const uint32_t sectorSize = header.sectorSize;

	// miniFAT
	const uint32_t numMiniFatSectors = 1;
	cfb::UintVec miniFatSectorNumVec;
	uint32_t miniFatNum; // num of mini fat sector
	uint32_t miniFatDataSectorNum; // sector num for mini fat data
	{
		// streamObject
		const int tbl[] = {
			iVersion, iDataSpaceMap, iStongEncryptionDataSpace, iPrimary, iEncryptionInfo
		};
		uint32_t pos = 0;
		for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
			uint32_t n = getMiniFatSectorNum(dirs[tbl[i]].streamSize);
			miniFatSectorNumVec.push_back(n);
			dirs[tbl[i]].startingSectorLocation = pos;
			pos += n;
		}
		miniFatNum = pos;
		dprintf("miniFatNum=%d\n", miniFatNum);
		miniFatDataSectorNum = getBlockNum(miniFatNum, (sectorSize / 64));
		dprintf("miniFatDataSectorNum=%d\n", miniFatDataSectorNum);
		if (getBlockNum(miniFatDataSectorNum, 128u) > numMiniFatSectors) throw cybozu::Exception("ms:makeLayout:large miniFatDataSectorNum") << miniFatDataSectorNum;
	}

	const uint32_t directoryEntrySectorNum = getBlockNum(static_cast<uint32_t>(cfb.dirs.size()), 4u);
	const uint32_t encryptionPackageSectorNum = getBlockNum(static_cast<uint32_t>(dirs[iEncryptionPackage].streamSize), sectorSize);
	const uint32_t contentSectorNum = numMiniFatSectors + directoryEntrySectorNum + miniFatDataSectorNum + encryptionPackageSectorNum;
dprintf("encryptionPackageSectorNum=%u\n", encryptionPackageSectorNum);
dprintf("contentSectorNum=%u\n", contentSectorNum);

	uint32_t fatSectorNum;
	uint32_t difatSectorNum;

	detectSectorNum(difatSectorNum, fatSectorNum, contentSectorNum, sectorSize);
dprintf("fatSectorNum=%u\n", fatSectorNum);
dprintf("difatSectorNum=%u\n", difatSectorNum);

	const uint32_t difatPos = 0;
	const uint32_t fatPos = difatPos + difatSectorNum;
	const uint32_t miniFatPos = fatPos + fatSectorNum;
	const uint32_t directoryEntryPos = miniFatPos + numMiniFatSectors;
	const uint32_t miniFatDataPos = directoryEntryPos + directoryEntrySectorNum;
	const uint32_t encryptionPackagePos = miniFatDataPos + miniFatDataSectorNum;

	header.firstDirectorySectorLocation = directoryEntryPos;
	header.firstMiniFatSectorLocation = miniFatPos;
	header.numMiniFatSectors = numMiniFatSectors;

	// FAT
	dirs[iRoot].startingSectorLocation = miniFatDataPos;
	dirs[iRoot].streamSize = 64 * miniFatNum;
	dirs[iEncryptionPackage].startingSectorLocation = encryptionPackagePos;

	const uint32_t allSectorNum = difatSectorNum + fatSectorNum + contentSectorNum;
	dprintf("allSectorNum=%u\n", allSectorNum);
	data.resize(512 + allSectorNum * sectorSize);

	for (uint32_t i = 0; i < std::min<uint32_t>(fatSectorNum, cfb::Header::firstNumDIFAT); i++) {
		header.difat.push_back(fatPos + i);
	}
	header.numFatSectors = fatSectorNum;
	header.numDifatSectors = difatSectorNum;
	if (difatSectorNum > 0) {
		header.firstDifatSectorLocation = difatPos;
	}
	header.write(&data[0]);

	// make DIFAT
	if (difatSectorNum > 0) {
		makeDifat(&data[0], difatPos, difatSectorNum, fatSectorNum, sectorSize);
	}

	// make FAT
	{
		cfb::UintVec v;
		for (uint32_t i = 0; i < difatSectorNum; i++) {
			v.push_back(cfb::DIFSECT);
		}
		for (uint32_t i = 0; i < fatSectorNum; i++) {
			v.push_back(cfb::FATSECT);
		}
		v.push_back(numMiniFatSectors);
		v.push_back(directoryEntrySectorNum);
		v.push_back(miniFatDataSectorNum);
		v.push_back(encryptionPackageSectorNum);
		char *p = &data[512 + fatPos * sectorSize];
		makeFat(p, p + fatSectorNum * sectorSize, v);
	}
	// make mini FAT
	{
		char *p = &data[512 + miniFatPos * sectorSize];
		makeFat(p, p + numMiniFatSectors * sectorSize, miniFatSectorNumVec);
	}
	for (size_t i = 0; i < dirs.size(); i++) {
		dirs[i].write(&data[512 + directoryEntryPos * sectorSize + i * 128]);
	}
	for (size_t i = 0; i < dirs.size(); i++) {
		const cfb::DirectoryEntry& dir = dirs[i];
		const size_t size = dir.content.size();
		if (size == 0) continue;
		if (size <= 4096) {
			memcpy(&data[512 + miniFatDataPos * sectorSize + dir.startingSectorLocation * 64], dir.content.c_str(), size);
		} else {
			memcpy(&data[512 + dir.startingSectorLocation * sectorSize], dir.content.c_str(), size);
		}
	}
	dprintf("data.size=%d\n", (int)data.size());
}

} // ms
