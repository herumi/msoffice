include ../common.mk

TARGET=$(TEST_FILE)

SRC=msoffice-crypt.cpp attack.cpp

all: $(TARGET) $(SRC)

test: $(TARGET)
	@$(SAMPLE_TEST)

$(OBJDIR):
	@$(MKDIR) $(OBJDIR)

clean:
	$(CLEAN)

-include $(DEPEND_FILE)

