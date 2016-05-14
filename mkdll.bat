set OPT=/Ox /EHsc /W3 -I .\include -I ..\cybozulib\include -I ..\cybozulib_ext\include -DNOMINMAX -DNDEBUG
cl /LD src\msocdll.cpp %OPT% /Febin\msoc.dll /link /LIBPATH:..\cybozulib_ext\lib
cl /MT src\msocsample.c %OPT% /Febin\msocsample.exe /link /LIBPATH:bin
