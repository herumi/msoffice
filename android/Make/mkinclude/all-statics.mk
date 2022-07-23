########################################################################
# sdk-statics.mk ABr
# Static variables for a specific SDK (e.g. iphoneos / iphonesimulator)

# overall build output
PRJ_DIR:=$(realpath .)
BUILD_DIR:=$(PRJ_DIR)/build_dir
TOP_DIR:=$(realpath ../../..)
SIMDE_DIR:=$(TOP_DIR)/simde
MSOC_DIR:=$(TOP_DIR)/msoffice
OPENSSL_DIR:=$(MSOC_DIR)/android/openssl

# all other locations are relative to MSOC_DIR
SRC_DIR:=$(MSOC_DIR)/src

# set API level here
export ANDROID_API:=29

# require NDK via Xamarin
XAMARIN_ANDROID_SDK_DIR=$(HOME)/Library/Developer/Xamarin/android-sdk-macosx
XAMARIN_ANDROID_NDK_DIR=$(XAMARIN_ANDROID_SDK_DIR)/ndk-bundle
ifeq ($(wildcard $(XAMARIN_ANDROID_NDK_DIR)/.*),)
$(error This build requires Xamarin Android NDK to be installed at '$(XAMARIN_ANDROID_NDK_DIR)')
endif
#$(info XAMARIN_ANDROID_NDK_DIR=$(XAMARIN_ANDROID_NDK_DIR))

# note tools specific to mac and intel :(
XAMARIN_ANDROID_NDK_TOOLCHAIN_DIR:=$(XAMARIN_ANDROID_NDK_DIR)/toolchains
XAMARIN_ANDROID_NDK_PREBUILT_DIR:=$(XAMARIN_ANDROID_NDK_TOOLCHAIN_DIR)/llvm/prebuilt/darwin-x86_64
XAMARIN_ANDROID_NDK_SYSROOT_DIR:=$(XAMARIN_ANDROID_NDK_PREBUILT_DIR)/sysroot

# set major env vars used to locate tools
ifeq (,$(findstring $(XAMARIN_ANDROID_SDK_DIR)/emulator,$(PATH)))
PATH:=$(PATH):$(XAMARIN_ANDROID_SDK_DIR)/emulator
endif
ifeq (,$(findstring $(XAMARIN_ANDROID_SDK_DIR)/tools/bin,$(PATH)))
PATH:=$(PATH):$(XAMARIN_ANDROID_SDK_DIR)/tools/bin
endif
ifeq (,$(findstring $(XAMARIN_ANDROID_SDK_DIR)/platform-tools,$(PATH)))
PATH:=$(PATH):$(XAMARIN_ANDROID_SDK_DIR)/platform-tools
endif
ifeq (,$(findstring $(XAMARIN_ANDROID_NDK_PREBUILT_DIR)/bin,$(PATH)))
PATH:=$(PATH):$(XAMARIN_ANDROID_NDK_PREBUILT_DIR)/bin
endif
export PATH
#$(error PATH=$(PATH))
ANDROID_NDK_HOME:=$(XAMARIN_ANDROID_NDK_DIR)
export ANDROID_NDK_HOME
PLATFORM_TYPE:=Android
export PLATFORM_TYPE

