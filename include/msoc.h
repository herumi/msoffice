#pragma once
#include <stdlib.h>
/**
	@file
	@brief MS Office Crypt tool dll
	@author herumi
	Copyright (C) 2016 Cybozu Labs, Inc., all rights reserved.
*/

#ifndef MSOC_DONT_AUTO_LINK
#pragma comment(lib, "msoc.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MSOC_NOERR 0
#define MSOC_ERR_NOT_SUPPORT (-1)
#define MSOC_ERR_ALREADY_ENCRYPTED (-2)
#define MSOC_ERR_ALREADY_DECRYPTED (-3)
#define MSOC_ERR_BAD_PASSWORD (-4)
#define MSOC_ERR_BAD_PARAMETER (-5)
#define MSOC_ERR_SMALL_MAX_SIZE (-6)
#define MSOC_ERR_NO_MEMORY (-7)
#define MSOC_ERR_EXCEPTION (-8)
#define MSOC_ERR_TOO_LARGE_FILE (-9)
#define MSOC_ERR_INFILE_IS_EMPTY (-10)
#define MSOC_ERR_OUTFILE_IS_EMPTY (-11)
#define MSOC_ERR_PASS_IS_EMPTY (-12)

__declspec(dllexport) const char *MSOC_getErrMessage(int err);

typedef struct msoc_opt msoc_opt;

/*
	encrypt inFile and make outFile with pass
	@param outFile [in] encrypted MS Office file
	@param inFile [in] plain MS Office file
	@param pass [in] password
	@param opt [inout] option (NULL is permitted)
	use spinCount, secretKey of opt if set
*/
__declspec(dllexport) int MSOC_encrypt(const wchar_t *outFile, const wchar_t *inFile, const wchar_t *pass, const msoc_opt *opt);
/*
	decrypt inFile and make outFile with pass
	@param outFile [in] plain MS Office file (NULL is permitted)
	@param inFile [in] encrypted MS Office file
	@param pass [in] password (NULL is permitted)
	@param opt [inout] option (NULL is permitted)

	secretKey of opt is used if pass is NULL
	opt is set by the value of inFile if opt is not NULL
*/
__declspec(dllexport) int MSOC_decrypt(const wchar_t *outFile, const wchar_t *inFile, const wchar_t *pass, msoc_opt *opt);

/*
	optType of msoc_opt
*/
#define MSOC_OPT_TYPE_SPIN_COUNT 1 // spinCount
#define MSOC_OPT_TYPE_SECRET_KEY 2 // secretKey(hex string) (eg. 0123ab)

/*
	create default msoc_opt
	spinCount = 100000
	secretKey = "" (auto generated)
	return pointer to msoc_opt
	err if NULL
*/
__declspec(dllexport) msoc_opt *MSOC_createOpt(void);
/*
	destroy msoc_opt
*/
__declspec(dllexport) void MSOC_destroyOpt(msoc_opt *msoc);

/*
	get int value of optType of opt);
	@param value [out] return value
*/
__declspec(dllexport) int MSOC_getInt(int *value, const msoc_opt *opt, int optType);
/*
	set int value of optType of opt
*/
__declspec(dllexport) int MSOC_setInt(msoc_opt *opt, int optType, int value);
/*
	get string value of optType of opt
	@param str [out] return value
	@param maxSize [in] buffer size of str including '\0'
*/
__declspec(dllexport) int MSOC_getStr(char *str, size_t maxSize, const msoc_opt *opt, int optType);
/*
	set string value of optType of opt
*/
__declspec(dllexport) int MSOC_setStr(msoc_opt *opt, int optType, const char *str);

#ifdef __cplusplus
}
#endif
