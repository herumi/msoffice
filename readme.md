# decoder/encoder of Microsoft Office Document Cryptography

# Environment

* 64-bit Windows Visual Studio 2012
* gcc 4.6, clang 3.0 or later

# How to make

Linux

    mkdir work
    git clone https://github.com/herumi/cybozulib
    git clone https://github.dev.cybozu.co.jp/herumi/msoffice
    cd msoffice
    make -j RELEASE=1

Windows

    mkdir work
    git clone https://github.com/herumi/cybozulib
    git clone https://github.dev.cybozu.co.jp/herumi/msoffice
    git clone https://github.com/herumi/cybozulib_ext # for openssl
    cd msoffice
    open msoffice12.sln and build

# How to use

    msoffice-crypt.exe [-d|-e][-o opt][-v][-h] (-p|-ph8|-ph16) pass input [output]
     -d|-e  : decode / encode
     -o opt : 0:use AES128(default), 1: use AES256 for encoding
     -h     : print help
     -p     : password in only ascii
     -ph8   : password in utf8 hex. ex. 68656C6C6F for 'hello'
     -ph16  : password in utf16 hex. ex. u3042u3044u3046 for 'あいう'
     input  : input MS Office file
     output : output MS Office file
     -v     : print debug info

# Return code

* 0 success
* 1 not support formart
* 2 already encrypted with -e or decrypted with -d
* 3 bad password with -d

# Support format

Office 2010 or later Office Document format which suffix is pptx, docx, xlsx.

# References

* Compound File Binary File Format(v20120328)
[[MS-CFB]](http://msdn.microsoft.com/en-us/library/dd942138.aspx)
* Office Document Cryptography Structure Specification(v20120412)
[[MS-OFFCRYPTO]](http://msdn.microsoft.com/en-us/library/cc313071.aspx)

http://stricture-group.com/files/adobe-top100.txt
