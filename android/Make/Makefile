########################################################################
# Makefile, ABr
# Main build driver for all libraries.
# Individual ARCH are created using a specific Makefile to
# enable architecture-specific compilation.

########################################################################
# library identity
LIBNAME=msoc
#
# architectures to build
ALL_ARCHS?=x86 x86_64 arm64-v8a armeabi-v7a

# support code
include ./mkinclude/all-statics.mk

.PHONY: all build clean veryclean rebuild kick dirs

all: build

build clean rebuild kick: dirs
	@ for i in $(ALL_ARCHS) ; do \
      LIBNAME="$(LIBNAME)" ; \
      ARCH="$$i" ; \
      export LIBNAME ARCH ; \
      echo "***make $$i" ; \
      $(MAKE) --no-print-directory -f ./mkinclude/Makefile.arch $@ ; \
      rc=$$? ; [ $$rc -ne 0 ] && exit $$rc ; \
      echo ; \
    done

veryclean:
	rm -fR "$(BUILD_DIR)"

dirs: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p "$@"
