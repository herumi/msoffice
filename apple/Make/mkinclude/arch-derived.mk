########################################################################
# arch-derived.mk ABr
# Derived variables for a specific architecture

ifeq ($(ARCH),)
$(error Must define ARCH to invoke this Makefile)
endif

# iOS version is different for i386 - max at 10.0
THE_IOS_MINIMUM_VERSION:=$(IOS_MINIMUM_VERSION)
ifeq ($(ARCH),i386)
THE_IOS_MINIMUM_VERSION:=10.0
endif

# build output for this particular sdk / architecture
ARCH_BUILD_DIR:=$(SDK_BUILD_DIR)/$(ARCH)
ARCH_OBJ_DIR:=$(ARCH_BUILD_DIR)/obj
ARCH_LIB_DIR:=$(ARCH_BUILD_DIR)/lib
ARCH_LIB_PATH:=$(ARCH_LIB_DIR)/lib$(LIBNAME).a

# dylib only for macosx
ifeq ($(SDK),macosx)
ARCH_DYLIB_PATH:=$(ARCH_LIB_DIR)/lib$(LIBNAME).dylib
endif

# determine how to locate openssl library as it is not provided for ios
ifeq ($(SDK),iphoneos)
TC3_LIBRARY_SDKNAME:=iPhone
CFLAG_IOS_MIN_VERSION:=-mios-version-min=$(THE_IOS_MINIMUM_VERSION)
endif
ifeq ($(SDK),iphonesimulator)
TC3_LIBRARY_SDKNAME:=iPhoneSimulator
CFLAG_IOS_MIN_VERSION:=-mios-simulator-version-min=$(THE_IOS_MINIMUM_VERSION)
endif
ifeq ($(SDK),macosx)
TC3_LIBRARY_SDKNAME:=macOS
endif
ifneq ($(TC3_LIBRARY_SDKNAME),)
LDFLAGS:=-lc++ -L$(MSOC_DIR)/../openssl/apple/$(TC3_LIBRARY_SDKNAME) -lcrypto -lssl
endif

# compiler and flags
CC:=$(shell xcrun --sdk $(SDK) --find clang)
SYSROOT:=$(shell xcrun --sdk $(SDK) --show-sdk-path)
CFLAGS:=$(GLOBAL_CFLAGS) -isysroot $(SYSROOT) -arch $(ARCH) -fobjc-arc -Werror -D tc3_$(ARCH) $(CFLAG_IOS_MIN_VERSION)
LIBTOOLFLAGS:=-arch_only $(ARCH)
DYLIBTOOLFLAGS:=-dynamiclib -install_name lib$(LIBNAME).dylib -L$(SDK_XCODE_PLATFORM_SDK_PATH)/usr/lib -F$(SDK_XCODE_PLATFORM_SDK_PATH)/System/Library/Frameworks -framework CoreFoundation -framework Foundation -framework Security

# list of source files and objects (base names only)
SRCS:=$(shell find $(SRC_DIR) -type f -name 'msocdll.cpp' -maxdepth 1 -exec basename {} \;)
OBJS:=$(SRCS:.cpp=.o)

# actual objects for this architecture
ARCH_OBJS:=$(foreach obj,$(OBJS),$(ARCH_OBJ_DIR)/$(obj))
