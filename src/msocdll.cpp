/**
	@file
	@brief MS Office Crypt DLL
	@author herumi
	Copyright (C) 2016 Cybozu Labs, Inc., all rights reserved.
*/
#pragma warning(disable : 4456)
#pragma warning(disable : 4458)

#define MSOC_DONT_AUTO_LINK
#include "cfb.hpp"
#include "decode.hpp"
#include "encode.hpp"
#include "make_dataspace.hpp"
#include "msoc.h"

static const size_t maxExceptionSize = 1024;
char g_exception[maxExceptionSize + 1] = "exception";

void setException(const std::exception& e)
{
	/// QQQ : not multi-thread
	size_t len = strlen(e.what());
	if (len >= maxExceptionSize) len = maxExceptionSize;
	memcpy(g_exception, e.what(), len);
	g_exception[len] = '\0';
}

__declspec(dllexport) const char *MSOC_getErrMessage(int err)
{
	switch (err) {
	case MSOC_NOERR:
		return "noerr";
	case MSOC_ERR_NOT_SUPPORT:
		return "not supported format";
	case MSOC_ERR_ALREADY_ENCRYPTED:
		return "already encrypted";
	case MSOC_ERR_ALREADY_DECRYPTED:
		return "already decrypted";
	case MSOC_ERR_BAD_PASSWORD:
		return "bad password";
	case MSOC_ERR_NO_MEMORY:
		return "no memory";
	case MSOC_ERR_EXCEPTION:
		return g_exception;
	case MSOC_ERR_TOO_LARGE_FILE:
		return "too large file";
	case MSOC_ERR_INFILE_IS_EMPTY:
		return "inFile is empty";
	case MSOC_ERR_OUTFILE_IS_EMPTY:
		return "outFile is empty";
	case MSOC_ERR_PASS_IS_EMPTY:
		return "pass is empty";
	default:
		return "unknown err";
	}
}

struct msoc_opt {
	int spinCount;
	std::string secretKey;
	msoc_opt()
		: spinCount(100000)
		, secretKey()
	{
	}
};

__declspec(dllexport) msoc_opt *MSOC_createOpt(void)
{
	try {
		return new msoc_opt();
	} catch (...) {
		return NULL;
	}
}

__declspec(dllexport) void MSOC_destroyOpt(msoc_opt *msoc)
{
	delete(msoc);
}

__declspec(dllexport) int MSOC_getInt(int *value, const msoc_opt *opt, int optType)
{
	switch (optType) {
	case MSOC_OPT_TYPE_SPIN_COUNT:
		*value = opt->spinCount;
		return MSOC_NOERR;
	default:
		return MSOC_ERR_BAD_PARAMETER;
	}
}

__declspec(dllexport) int MSOC_setInt(msoc_opt *opt, int optType, int value)
{
	switch (optType) {
	case MSOC_OPT_TYPE_SPIN_COUNT:
		opt->spinCount = value;
		return MSOC_NOERR;
	default:
		return MSOC_ERR_BAD_PARAMETER;
	}
}

static int getStr(char *str, size_t maxSize, const std::string& s)
{
	if (maxSize < s.size() + 1) return MSOC_ERR_SMALL_MAX_SIZE;
	memcpy(str, s.c_str(), s.size());
	str[s.size()] = '\0';
	return MSOC_NOERR;
}

__declspec(dllexport) int MSOC_getStr(char *str, size_t maxSize, const msoc_opt *opt, int optType)
{
	switch (optType) {
	case MSOC_OPT_TYPE_SECRET_KEY:
		return getStr(str, maxSize, opt->secretKey);
	default:
		return MSOC_ERR_BAD_PARAMETER;
	}
}

__declspec(dllexport) int MSOC_setStr(msoc_opt *opt, int optType, const char *str)
	try
{
	switch (optType) {
	case MSOC_OPT_TYPE_SECRET_KEY:
		opt->secretKey = str;
		return MSOC_NOERR;
	default:
		return MSOC_ERR_BAD_PARAMETER;
	}
} catch (...) {
	return MSOC_ERR_NO_MEMORY;
}

static int readFile(std::string& data, ms::Format& format, uint32_t& dataSize, const wchar_t *inFile)
{
	cybozu::Mmap m(inFile);
	if (m.size() > 0xffffffff) {
		return MSOC_ERR_TOO_LARGE_FILE;
	}
	dataSize = static_cast<uint32_t>(m.size());
	data.assign(m.get(), dataSize);
	format = ms::DetectFormat(data.data(), dataSize);
	return MSOC_NOERR;
}

__declspec(dllexport) int MSOC_encrypt(const wchar_t *outFile, const wchar_t *inFile, const wchar_t *pass, const msoc_opt *opt)
	try
{
	if (outFile == NULL) return MSOC_ERR_OUTFILE_IS_EMPTY;
	if (inFile == NULL) return MSOC_ERR_INFILE_IS_EMPTY;
	if (pass == NULL) return MSOC_ERR_PASS_IS_EMPTY;
	const bool isOffice2013 = true;
	std::string data;
	ms::Format format;
	uint32_t dataSize;
	int err = readFile(data, format, dataSize, inFile);
	if (err != MSOC_NOERR) return err;
	if (format == ms::fCfb) {
		return MSOC_ERR_ALREADY_ENCRYPTED;
	}
	int spinCount = 100000;
	std::string secretKey;
	if (opt) {
		if (opt->spinCount) spinCount = opt->spinCount;
		if (!opt->secretKey.empty()) secretKey = ms::fromHex(opt->secretKey);
	}
	std::wstring outFileW = outFile;
	std::string passData = ms::Char16toChar8(pass);
	ms::encode(data.data(), dataSize, outFileW, passData, isOffice2013, secretKey, spinCount);
	return MSOC_NOERR;
} catch (std::exception& e) {
	setException(e);
	return MSOC_ERR_EXCEPTION;
}

__declspec(dllexport) int MSOC_decrypt(const wchar_t *outFile, const wchar_t *inFile, const wchar_t *pass, msoc_opt *opt)
	try
{
	if (inFile == NULL) return MSOC_ERR_INFILE_IS_EMPTY;
	const bool doView = outFile == NULL;
	std::string data;
	ms::Format format;
	uint32_t dataSize;
	int err = readFile(data, format, dataSize, inFile);
	if (err != MSOC_NOERR) return err;
	if (format == ms::fZip) {
		return MSOC_ERR_ALREADY_DECRYPTED;
	}
	std::string secretKey;
	if (opt) {
		if (!opt->secretKey.empty()) secretKey = ms::fromHex(opt->secretKey);
	}
	std::wstring outFileW;
	if (outFile) {
		outFileW = outFile;
	}
	int spinCount;
	std::string passData;
	if (pass) passData = ms::Char16toChar8(pass);
	bool ok = ms::decode(data.data(), dataSize, outFileW, passData, secretKey, doView, &spinCount);
	if (!ok) return MSOC_ERR_BAD_PASSWORD;
	if (opt) {
		opt->spinCount = spinCount;
		opt->secretKey = ms::hex(secretKey);
	}
	return MSOC_NOERR;
} catch (std::exception& e) {
	setException(e);
	return MSOC_ERR_EXCEPTION;
}
