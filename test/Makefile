include ../common.mk

TARGET=$(TEST_FILE)
LIBS=

SRC=$(wildcard *.cpp)

all: $(TARGET)

test: $(TARGET)
	@$(UNIT_TEST)

$(OBJDIR):
	@$(MKDIR) $(OBJDIR)

clean:
	$(CLEAN)

-include $(DEPEND_FILE)

