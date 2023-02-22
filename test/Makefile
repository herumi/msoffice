include ../common.mk

TARGET=$(TEST_FILE)
LIBS=

SRC=cfb_test.cpp
ifeq ($(shell uname -m),x86_64)
  SRC+=hash_test.cpp
endif

all: $(TARGET)

test: $(TARGET)
	@$(UNIT_TEST)

$(OBJDIR):
	@$(MKDIR) $(OBJDIR)

clean:
	$(CLEAN)

-include $(DEPEND_FILE)

