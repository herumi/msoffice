/**
	@file
	@brief mini MS Office Crypt DLL sample
	@author herumi
	Copyright (C) 2016 Cybozu Labs, Inc., all rights reserved.
*/
#include "msoc.h"
#include <stdio.h>
#include <string.h>

void usage()
{
	const char *s =
	"how to use\n"
	"minisample enc <inFile> <outFile> <pass>\n"
	"minisample dec <inFile> <outFile> <pass>\n";
	printf("%s", s);
	exit(1);
}

int main(int argc, char **argv)
{
	int err = 0;
	const char *mode = NULL;
	const char *inFile = NULL;
	const char *outFile = NULL;
	const char *pass = NULL;

	argc--, argv++;
	if (argc != 4) {
		usage();
	}
	mode = argv[0];
	inFile = argv[1];
	outFile = argv[2];
	pass = argv[3];
	printf("enc in=%s, out=%s, pass=%s\n", inFile, outFile, pass);

	if (strcmp(mode, "enc") == 0) {
		err = MSOC_encryptA(outFile, inFile, pass, NULL);
	} else if (strcmp(mode, "dec") == 0) {
		err = MSOC_decryptA(outFile, inFile, pass, NULL);
	} else {
		printf("base mode=%s\n", mode);
		usage();
	}
	if (err != MSOC_NOERR) {
		printf("ERR %s\n", MSOC_getErrMessage(err));
		return 1;
	} else {
		puts("ok");
	}
	return 0;
}
