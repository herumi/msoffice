CP = cp -f
AR = ar r
MKDIR=mkdir -p
RM=rm -fr
CFLAGS_OPT = -Ofast -fomit-frame-pointer -DNDEBUG -march=native -msse4
#CFLAGS_WARN=-Wall -Wextra -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith #-Wswitch-enum -Wstrict-aliasing=2
CFLAGS_WARN=-Wall -Wextra -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wfloat-equal -Wpointer-arith #-Wswitch-enum -Wstrict-aliasing=2
CFLAGS = -g -D_FILE_OFFSET_BITS=64 -msse2 -fno-operator-names
CFLAGS+=$(CFLAGS_WARN)
LDFLAGS = -lcrypto -lssl
UNAME := $(shell uname)

DEBUG=0
ifeq ($(RELEASE),1)
	DEBUG=0
endif

ifeq ($(DEBUG),0)
	CFLAGS+=$(CFLAGS_OPT)
	OBJDIR=release
	OBJSUF=
else
	OBJDIR=debug
	OBJSUF=d
endif

TOPDIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
CFLAGS+= -I$(TOPDIR)include -I$(TOPDIR)../cybozulib/include -I$(TOPDIR)../mie/include
LDFLAGS+= -L$(TOPDIR)lib -lcrypto -lpthread

ifeq ($(UNAME), Darwin)
	LDFLAGS+= -DNORT
else
	LDFLAGS+= -lrt
endif

MKDEP = sh -ec '$(CC) -MM $(CFLAGS) $< | sed "s@\($*\)\.o[ :]*@$(OBJDIR)/\1.o $@ : @g" > $@; [ -s $@ ] || rm -f $@'

CLEAN=$(RM) $(TARGET) $(OBJDIR)

define UNIT_TEST
sh -ec 'for i in $(TARGET); do $$i|grep "ctest:name"; done' > result.txt
grep -v "ng=0, exception=0" result.txt || echo "all unit tests are ok"
endef

define SAMPLE_TEST
sh -ec 'for i in $(TARGET); do $$i; done'
endef

.SUFFIXES: .cpp .d .exe

$(OBJDIR)/%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)

$(OBJDIR)/%.d: %.cpp $(OBJDIR)
	@$(MKDEP)

$(TOPDIR)bin/%$(OBJSUF).exe: $(OBJDIR)/%.o $(LIBS)
	$(CXX) $< -o $@ $(LIBS) $(LDFLAGS)

OBJS=$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

DEPEND_FILE=$(addprefix $(OBJDIR)/, $(SRC:.cpp=.d))
TEST_FILE=$(addprefix $(TOPDIR)bin/, $(SRC:.cpp=$(OBJSUF).exe))

.PHONY: test

