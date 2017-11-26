# -*- coding: utf-8 -*-
#	@file
#	@brief MS Office Crypt DLL sample
#	@author herumi
#	Copyright (C) 2016 Cybozu Labs, Inc., all rights reserved.
import os, sys
import argparse
from ctypes import *
from ctypes.wintypes import LPWSTR, LPCSTR, LPVOID

MSOC_OPT_TYPE_SPIN_COUNT = 1
MSOC_OPT_TYPE_SECRET_KEY = 2

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

def putErrMsg(lib, err):
	MSOC_getErrMessage = lib.MSOC_getErrMessage
	MSOC_getErrMessage.restype = LPCSTR
	print 'ERR', MSOC_getErrMessage(err)
	os._exit(1)

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
		putErrMsg(lib, err)
	return intv.value

def _setInt(lib, opt, optType, v):
	err = lib.MSOC_setInt(opt, optType, v)
	if err:
		putErrMsg(lib, err)

def _getStr(lib, opt, optType):
	svLen = 256
	sv = create_string_buffer('\0' * svLen)
	err = lib.MSOC_getStr(sv, svLen, opt, optType)
	if err:
		putErrMsg(lib, err)
	return sv.value

def _setStr(lib, opt, optType, s):
	cs = c_char_p(s)
	err = lib.MSOC_setStr(opt, optType, cs)
	if err:
		putErrMsg(lib, err)

class Msoc:
	def __init__(self):
		if sys.maxsize > 2**32:
			dll = 'bin/64/msoc.dll'
		else:
			dll = 'bin/msoc.dll'
		self.lib = windll.LoadLibrary(dll)
		self.opt = _createOpt(self.lib)
	def __del__(self):
		_destroyOpt(self.lib, self.opt)

	def encrypt(self, outFile, inFile, ps):
		outFileW = c_wchar_p(outFile)
		inFileW = c_wchar_p(inFile)
		psW = c_wchar_p(ps)
		# permit self.opt is None
		err = self.lib.MSOC_encrypt(outFileW, inFileW, psW, self.opt)
		if err:
			putErrMsg(self.lib, err)

	def decrypt(self, outFile, inFile, ps):
		if outFile:
			outFileW = c_wchar_p(outFile)
		else:
			outFileW = None
		inFileW = c_wchar_p(inFile)
		psW = c_wchar_p(ps)
		# permit self.opt is None
		err = self.lib.MSOC_decrypt(outFileW, inFileW, psW, self.opt)
		if err:
			putErrMsg(self.lib, err)

	def getOpt(self, optType):
		if optType in [MSOC_OPT_TYPE_SPIN_COUNT]:
			return _getInt(self.lib, self.opt, optType)
		elif optType in [MSOC_OPT_TYPE_SECRET_KEY]:
			return _getStr(self.lib, self.opt, optType)
		else:
			raise Exception('getOpt not support type', optType)

	def setOpt(self, optType, v):
		if optType in [MSOC_OPT_TYPE_SPIN_COUNT]:
			return _setInt(self.lib, self.opt, optType, v)
		elif optType in [MSOC_OPT_TYPE_SECRET_KEY]:
			return _setStr(self.lib, self.opt, optType, v)
		else:
			raise Exception('setOpt not support type', optType)

def main():
	parser = argparse.ArgumentParser(description='msocsample.py')
	sub = parser.add_subparsers(dest='mode')
	enc = sub.add_parser('enc', help='encrypt')
	enc.add_argument('inFile')
	enc.add_argument('outFile')
	enc.add_argument('ps', help='password')
	enc.add_argument('-sc', '--spinCount', help='spinCount')
	enc.add_argument('-sk', '--secretKey', help='secretKey')

	dec = sub.add_parser('dec', help='decrypt')
	dec.add_argument('inFile')
	dec.add_argument('outFile')
	dec.add_argument('ps', help='password')

	opt = sub.add_parser('opt', help='view option')
	opt.add_argument('inFile')
	opt.add_argument('ps', help='password')

	dec_sk = sub.add_parser('dec-sk', help='decrypt by secret key')
	dec_sk.add_argument('inFile')
	dec_sk.add_argument('outFile')
	dec_sk.add_argument('secretKey')

	argv = getWargv()
	for pos in xrange(len(argv)):
		if argv[pos].find('msocsample.py') >= 0:
			argv = argv[pos + 1:]
			break

	arg = parser.parse_args(argv)

	msoc = Msoc()

	if getattr(arg, 'secretKey', None) and arg.secretKey:
		secretKey = wchar2ascii(arg.secretKey)
		msoc.setOpt(MSOC_OPT_TYPE_SECRET_KEY, secretKey)
	if getattr(arg, 'spinCount', None) and arg.spinCount:
		spinCount = int(wchar2ascii(arg.spinCount))
		msoc.setOpt(MSOC_OPT_TYPE_SPIN_COUNT, spinCount)

	if arg.mode == 'enc':
		msoc.encrypt(arg.outFile, arg.inFile, arg.ps)
	elif arg.mode == 'dec':
		msoc.decrypt(arg.outFile, arg.inFile, arg.ps)
	elif arg.mode == 'opt':
		msoc.decrypt(None, arg.inFile, arg.ps)
		print 'spinCount', msoc.getOpt(MSOC_OPT_TYPE_SPIN_COUNT)
		print 'secretKey', msoc.getOpt(MSOC_OPT_TYPE_SECRET_KEY)
	elif arg.mode == 'dec-sk':
		msoc.decrypt(arg.outFile, arg.inFile, None)

if __name__ == '__main__':
	main()
