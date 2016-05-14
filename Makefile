include common.mk
all:
	-$(MKDIR) bin
	make -C src

#test:
#	make -C test test
test:
	python test_all.py
clean:
	make -C src clean
	make -C test clean

MSOC_LIB=lib/libmsoc.a
$(MSOC_LIB): src/msocdll.cpp include/msoc.h
	$(CXX) -c $(CFLAGS) -fPIC src/msocdll.cpp -o src/$(OBJDIR)/msocdll.o
	$(MKDIR) lib
	$(AR) $(MSOC_LIB) src/$(OBJDIR)/msocdll.o

bin/minisample: src/minisample.c include/msoc.h $(MSOC_LIB)
	$(CC) $< -o $@ $(MSOC_LIB) -lstdc++ -lcrypto -Iinclude -Wall -Wextra -lcrypto

