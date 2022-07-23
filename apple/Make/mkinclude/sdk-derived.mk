########################################################################
# sdk-derived.mk ABr
# Derived variables for a specific SDK (e.g. iphoneos / iphonesimulator)

ifeq ($(LIBNAME),)
$(error Must define LIBNAME to invoke this Makefile)
endif
ifeq ($(SDK),)
$(error Must define SDK to invoke this Makefile)
endif

# platform path used to locate framework / libraries for dylib link
XCODE_PATH:=$(shell xcode-select --print-path)
SDK_XCODE_PLATFORM_PATH:=$(shell find "$(XCODE_PATH)"/Platforms -maxdepth 1 -type d | grep -i -e '$(SDK)\.platform')
SDK_XCODE_PLATFORM_SDK_PATH:=$(shell find "$(SDK_XCODE_PLATFORM_PATH)"/Developer/SDKs -maxdepth 1 -type d | grep -i -e '$(SDK)\.sdk')

# build output for this particular sdk
SDK_BUILD_DIR:=$(BUILD_DIR)/$(SDK)

# overall output (universal library with all architectures)
UNIVERSAL_LIB_DIR:=$(SDK_BUILD_DIR)/lib
UNIVERSAL_LIB_PATH:=$(UNIVERSAL_LIB_DIR)/lib$(LIBNAME).a
UNIVERSAL_DYLIB_PATH:=$(UNIVERSAL_LIB_DIR)/lib$(LIBNAME).dylib

# all libraries from all architectures
SDK_ARCH_LIBS:=$(foreach arch,$(ARCHS_$(SDK)),$(SDK_BUILD_DIR)/$(arch)/lib/lib$(LIBNAME).a)
