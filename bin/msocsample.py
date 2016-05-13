# -*- coding: utf-8 -*-
#	@file
#	@brief MS Office Crypt DLL sample
#	@author herumi
#	Copyright (C) 2016 Cybozu Labs, Inc., all rights reserved.
import os, sys
from ctypes import *
from ctypes.wintypes import LPWSTR, LPCSTR, LPVOID

MSOC_OPT_TYPE_SPIN_COUNT = 1
MSOC_OPT_TYPE_SECRET_KEY = 2

def usage():
	s ='''how to use
msocsample.py enc <inFile> <outFile> <pass>
msocsample.py dec <inFile> <outFile> <pass>
msocsample.py opt <inFile> <pass>
msocsample.py enc-sec <inFile> <outFile> <pass> <secretKey> [<spinCount>]
msocsample.py dec-sec <inFile> <outFile> <secretKey>
'''
	print s
	os._exit(1)

def getWargv():
	cmd = cdll.kernel32.GetCommandLineW()
	argc = c_int(0)
	CommandLineToArgvW = windll.shell32.CommandLineToArgvW
	CommandLineToArgvW.restype = POINTER(LPWSTR)
	argv = CommandLineToArgvW(cmd, byref(argc))
	ret = []
	for i in xrange(argc.value):
		ret.append(argv[i])
	return ret

def wchar2ascii(w):
	s = ''
	for wc in w:
		c = chr(ord(wc))
		s += c
	return s

def _createOpt(lib):
	opt = c_void_p(0)
	MSOC_createOpt = lib.MSOC_createOpt
	MSOC_createOpt.restype = LPVOID
	opt = MSOC_createOpt()
	if opt:
		return opt
	print 'ERR MSOC_createOpt()'
	os._exit(1)

def _destroyOpt(lib, opt):
	lib.MSOC_destroyOpt(opt)

def _getInt(lib, opt, optType):
	intv = c_int(0)
	err = lib.MSOC_getInt(byref(intv), opt, optType)
	if err:
		putErrMsg(err)
	return intv.value

def _setInt(lib, opt, optType, v):
	err = lib.MSOC_setInt(opt, optType, v)
	if err:
		putErrMsg(err)

def _getStr(lib, opt, optType):
	svLen = 256
	sv = create_string_buffer('\0' * svLen)
	err = lib.MSOC_getStr(sv, svLen, opt, optType)
	if err:
		putErrMsg(err)
	return sv.value

def _setStr(lib, opt, optType, s):
	cs = c_char_p(s)
	err = lib.MSOC_setStr(opt, optType, cs)
	if err:
		putErrMsg(err)

class Msoc:
	def __init__(self):
		self.lib = cdll.LoadLibrary('bin/msoc.dll')
		self._opt = _createOpt(self.lib)
	def __del__(self):
		_destroyOpt(self.lib, self._opt)

	def putErrMsg(self, err):
		MSOC_getErrMessage = self.lib.MSOC_getErrMessage
		MSOC_getErrMessage.restype = LPCSTR
		print 'ERR', MSOC_getErrMessage(err)
		os._exit(1)

	def encrypt(self, outFile, inFile, ps):
		outFileW = c_wchar_p(outFile)
		inFileW = c_wchar_p(inFile)
		psW = c_wchar_p(ps)
		# permit self._opt is None
		err = self.lib.MSOC_encrypt(outFileW, inFileW, psW, self._opt)
		if err:
			self.putErrMsg(err)

	def decrypt(self, outFile, inFile, ps):
		if outFile:
			outFileW = c_wchar_p(outFile)
		else:
			outFileW = None
		inFileW = c_wchar_p(inFile)
		psW = c_wchar_p(ps)
		# permit self._opt is None
		err = self.lib.MSOC_decrypt(outFileW, inFileW, psW, self._opt)
		if err:
			self.putErrMsg(err)

	def getOpt(self, optType):
		if optType in [MSOC_OPT_TYPE_SPIN_COUNT]:
			return _getInt(self.lib, self._opt, optType)
		elif optType in [MSOC_OPT_TYPE_SECRET_KEY]:
			return _getStr(self.lib, self._opt, optType)
		else:
			raise Exception('getOpt not support type', optType)

	def setOpt(self, optType, v):
		if optType in [MSOC_OPT_TYPE_SPIN_COUNT]:
			return _setInt(self.lib, self._opt, optType, v)
		elif optType in [MSOC_OPT_TYPE_SECRET_KEY]:
			return _setStr(self.lib, self._opt, optType, v)
		else:
			raise Exception('setOpt not support type', optType)

def main():
	argv = getWargv()
	for pos in xrange(len(argv)):
		if argv[pos].find('msocsample.py') >= 0:
			argv = argv[pos + 1:]
			break
	argc = len(argv)

	if argc < 3:
		usage()

	msoc = Msoc()

	err = 0
	if argv[0] == 'dec':
		if argc != 4:
			usage()
		msoc.decrypt(argv[2], argv[1], argv[3])
	elif argv[0] == 'enc':
		if argc != 4:
			usage()
		msoc.encrypt(argv[2], argv[1], argv[3])
	elif argv[0] == 'opt':
		if argc != 3:
			usage()
		msoc.decrypt(None, argv[1], argv[2])
		print 'spinCount', msoc.getOpt(MSOC_OPT_TYPE_SPIN_COUNT)
		print 'secretKey', msoc.getOpt(MSOC_OPT_TYPE_SECRET_KEY)
	elif argv[0] == 'enc-sec':
		if argc != 5 and argc != 6:
			usage()
		secretKey = wchar2ascii(argv[4])
		msoc.setOpt(MSOC_OPT_TYPE_SECRET_KEY, secretKey)
		if argc == 6:
			spinCount = int(wchar2ascii(argv[5]))
			msoc.setOpt(MSOC_OPT_TYPE_SPIN_COUNT, spinCount)
		msoc.encrypt(argv[2], argv[1], argv[3])
	elif argv[0] == 'dec-sec':
		if argc != 4:
			usage()
		secretKey = wchar2ascii(argv[3])
		msoc.setOpt(MSOC_OPT_TYPE_SECRET_KEY, secretKey)
		msoc.decrypt(argv[2], argv[1], None)
	else:
		usage()

if __name__ == '__main__':
	main()
