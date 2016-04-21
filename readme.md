# decoder/encoder of Microsoft Office Document Cryptography

# Environment

* 64-bit Windows Visual Studio 2012 or later
* gcc 4.6, clang 3.0 or later

# How to make

Linux
```
    mkdir work
    git clone https://github.com/herumi/cybozulib
    git clone https://github.dev.cybozu.co.jp/herumi/msoffice
    cd msoffice
    make -j RELEASE=1
```
Windows
```
    mkdir work
    git clone https://github.com/herumi/cybozulib
    git clone https://github.dev.cybozu.co.jp/herumi/msoffice
    git clone https://github.com/herumi/cybozulib_ext # for openssl
    cd msoffice
    mk.bat ; or open msoffice12.sln and build
```
# How to use
```
usage:msoffice-crypt.exe [opt] input output
  -h : show this message
  -p password in only ascii
  -encMode 0:use AES128(default), 1: use AES256 for encoding
  -ph8 password in utf8 hex. ex. 68656C6C6F for 'hello'
  -ph16 password in utf16 hex. ex. u3042u3044u3046 for 'aiu' in hiragana
  -k (experimental) secret key in hex. ex. 0123456789ABCDEF0123456789ABCDEF
  -by (experimental) extract secret key from this file
  -e encode
  -d decode
  -c spin count
  -psk print secret key
  -v print debug info
  -vv print debug info and save binary data
```
# Return code

* 0 success
* 1 not support formart
* 2 already encrypted with -e or decrypted with -d
* 3 bad password with -d

# Support format

Office 2010 or later Office Document format which suffix is pptx, docx, xlsx.

# License
BSD 3-Clause License

Copyright (c) 2015 Cybozu Labs, Inc. All rights reserved.

# References

* Compound File Binary File Format(v20120328)
[[MS-CFB]](http://msdn.microsoft.com/en-us/library/dd942138.aspx)
* Office Document Cryptography Structure Specification(v20120412)
[[MS-OFFCRYPTO]](http://msdn.microsoft.com/en-us/library/cc313071.aspx)
* CODE BLUE 2015 [[Backdoors with the MS Office file encryption master key and a proposal for a reliable file format]](http://www.slideshare.net/herumi/backdoors-with-the-ms-office-file-encryption-master-key-and-a-proposal-for-a-reliable-file-format)
