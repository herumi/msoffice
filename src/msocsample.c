/**
	@file
	@brief MS Office Crypt DLL sample
	@author herumi
	Copyright (C) 2016 Cybozu Labs, Inc., all rights reserved.
*/
#include "msoc.h"
#include <stdio.h>

void usage()
{
	const char *s =
	"how to use\n"
	"msocsample.exe enc <inFile> <outFile> <pass>\n"
	"msocsample.exe dec <inFile> <outFile> <pass>\n"
	"msocsample.exe opt <inFile> <pass>\n"
	"msocsample.exe enc-sec <inFile> <outFile> <pass> <secretKey> [<spinCount>]\n"
	"msocsample.exe dec-sec <inFile> <outFile> <secretKey>\n";
	printf("%s", s);
	exit(1);
}

/*
	assume w[i] is ascii code
*/
void wchar2ascii(char *s, size_t maxSize, const wchar_t *w)
{
	size_t i;
	for (i = 0; i < maxSize; i++) {
		wchar_t c = w[i];
		s[i] = (char)c;
		if (s[i] != c) {
			printf("not ascii %04x\n", c);
			exit(1);
		}
		if (s[i] == '\0') return;
	}
	puts("too long w");
	exit(1);
}

int wmain(int argc, wchar_t **argv)
{
	int err = 0;
	const wchar_t *inFile = NULL;
	const wchar_t *outFile = NULL;
	struct msoc_opt *opt = NULL;
	int spinCount;
	char secretKey[256];

	argc--, argv++;
	if (argc < 3) {
		usage();
	}
	if (wcscmp(argv[0], L"enc") == 0) {
		if (argc != 4) {
			usage();
		}
		printf("enc in=%S, out=%S\n", argv[1], argv[2]);
		err = MSOC_encrypt(argv[2], argv[1], argv[3], NULL);
	} else
	if (wcscmp(argv[0], L"dec") == 0) {
		if (argc != 4) {
			usage();
		}
		printf("dec in=%S, out=%S, pass=%S\n", argv[1], argv[2], argv[3]);
		err = MSOC_decrypt(argv[2], argv[1], argv[3], NULL);
	} else
	if (wcscmp(argv[0], L"opt") == 0) {
		if (argc != 3) {
			usage();
		}
		printf("opt in=%S, pass=%S\n", argv[1], argv[2]);
		opt = MSOC_createOpt();
		if (opt == NULL) {
			puts("ERR MSOC_createOpt");
			return 1;
		}
		err = MSOC_decrypt(NULL, argv[1], argv[2], opt);
		if (err != MSOC_NOERR) goto EXIT;
		err = MSOC_getInt(&spinCount, opt, MSOC_OPT_TYPE_SPIN_COUNT);
		if (err != MSOC_NOERR) goto EXIT;
		err = MSOC_getStr(secretKey, sizeof(secretKey), opt, MSOC_OPT_TYPE_SECRET_KEY);
		if (err != MSOC_NOERR) goto EXIT;
		printf("spinCout=%d\n", spinCount);
		printf("secretKey=%s\n", secretKey);
	} else
	if (wcscmp(argv[0], L"enc-sec") == 0) {
		if (argc != 5 && argc != 6) {
			usage();
		}
		printf("enc-sec in=%S, out=%S, pass=%S, secretKey=%S\n", argv[1], argv[2], argv[3], argv[4]);
		wchar2ascii(secretKey, sizeof(secretKey), argv[4]);
		opt = MSOC_createOpt();
		if (opt == NULL) {
			puts("ERR MSOC_createOpt");
			return 1;
		}
		err = MSOC_setStr(opt, MSOC_OPT_TYPE_SECRET_KEY, secretKey);
		if (err != MSOC_NOERR) goto EXIT;
		if (argc == 6) {
			int spinCount = _wtoi(argv[5]);
			printf("spinCount=%d\n", spinCount);
			err = MSOC_setInt(opt, MSOC_OPT_TYPE_SPIN_COUNT, spinCount);
			if (err != MSOC_NOERR) goto EXIT;
		}
		err = MSOC_encrypt(argv[2], argv[1], argv[3], opt);
	} else
	if (wcscmp(argv[0], L"dec-sec") == 0) {
		if (argc != 4) {
			usage();
		}
		printf("dec-sec in=%S, out=%S, secretKey=%S\n", argv[1], argv[2], argv[3]);
		wchar2ascii(secretKey, sizeof(secretKey), argv[3]);
		opt = MSOC_createOpt();
		if (opt == NULL) {
			puts("ERR MSOC_createOpt");
			return 1;
		}
		err = MSOC_setStr(opt, MSOC_OPT_TYPE_SECRET_KEY, secretKey);
		if (err != MSOC_NOERR) goto EXIT;
		err = MSOC_decrypt(argv[2], argv[1], NULL, opt);
	} else
	{
		usage();
	}
EXIT:
	MSOC_destroyOpt(opt);
	if (err != MSOC_NOERR) {
		printf("ERR %s\n", MSOC_getErrMessage(err));
		return 1;
	} else {
		puts("ok");
	}
	return 0;
}
