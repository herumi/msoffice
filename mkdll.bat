set OPT=/Ox /EHsc /W3 -I .\include -DNOMINMAX -DNDEBUG
cl /LD src\msocdll.cpp msoc.def %OPT% /Febin\msoc.dll
cl /MT src\msocsample.c %OPT% /Febin\msocsample.exe /link /LIBPATH:bin
