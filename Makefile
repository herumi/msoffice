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

