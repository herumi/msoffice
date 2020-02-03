include common.mk

MSOC_LIB=lib/libmsoc.a
MSOC_SLIB=lib/libmsoc.$(LIB_SUF)

all:
	$(MAKE) -C src
	$(MAKE) $(MSOC_LIB)
	$(MAKE) $(MSOC_SLIB)

test:
	python test_all.py
clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	$(RM) -rf $(MSOC_LIB) $(MSOC_SLIB)

src/$(OBJDIR)/msocdll.o: src/msocdll.cpp include/msoc.h
	$(CXX) -c $< -o $@ $(CFLAGS) -fPIC

$(MSOC_LIB): src/$(OBJDIR)/msocdll.o
	$(AR) $@ $<

$(MSOC_SLIB): src/$(OBJDIR)/msocdll.o
	$(CXX) -o $@ $< -shared $(LDFLAGS)

bin/minisample: src/minisample.c include/msoc.h $(MSOC_LIB)
	$(CC) $< -o $@ $(MSOC_LIB) -lstdc++ -lcrypto -Iinclude -Wall -Wextra -lcrypto

