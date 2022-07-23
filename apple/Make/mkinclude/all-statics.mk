########################################################################
# all-statics.mk ABr
# Static variables for the entire build

# set iOS minimum version here
IOS_MINIMUM_VERSION=15.0

# overall build output
BUILD_DIR:=build_dir

# start with pointer to top-level msoc folder
MSOC_DIR:=../../..

# all other locations are relative to MSOC_DIR
SRC_DIR:=$(MSOC_DIR)/src/msoffice/src
GLOBAL_CFLAGS:=-I$(MSOC_DIR)/src/msoffice/include -I$(MSOC_DIR)/src/cybozulib/include -I$(MSOC_DIR)/../openssl/apple/include -O3

# iphoneos requires special flag
ifeq ($(SDK),iphoneos)
GLOBAL_CFLAGS:=$(GLOBAL_CFLAGS) -DARM64
endif

# known *supported* architectures for each SDK type
ARCHS_iphonesimulator:=i386 x86_64
ARCHS_iphoneos:=arm64 arm64e
ARCHS_macosx:=x86_64
