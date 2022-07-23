########################################################################
# Makefile, ABr
# Main build driver for universal library.
# Individual SDK / ARCH are created using a specific Makefile to
# enable architecture-specific compilation.

########################################################################
# library identity
LIBNAME=msoc
#
# xcodebuild -showsdks
#   iphoneos / iphonesimulator / macosx
ALL_SDKS?=iphonesimulator iphoneos

# support code
include ./mkinclude/all-statics.mk

.PHONY: all build clean veryclean rebuild kick dirs

all: build

build clean rebuild kick: dirs
	@ for i in $(ALL_SDKS) ; do \
      LIBNAME="$(LIBNAME)" ; \
      SDK="$$i" ; \
      export LIBNAME SDK ; \
      echo "***make $$i" ; \
      $(MAKE) --no-print-directory -f ./mkinclude/Makefile.sdk $@ ; \
      rc=$$? ; [ $$rc -ne 0 ] && exit $$rc ; \
      echo ; \
    done
	@ echo '***create universal library for binding...'
	@ mkdir -p build_dir/universal
	lipo -create \
    `find ./build_dir/iphonesimulator -type f -name '*.a' 2>/dev/null | grep -v 'iphonesimulator/lib'` \
    `find ./build_dir/iphoneos -type f -name '*.a' 2>/dev/null | grep -v 'iphoneos/lib'` \
    `find ./build_dir/macos -type f -name '*.a' 2>/dev/null | grep -v 'macos/lib'` \
    -output ./build_dir/universal/lib$(LIBNAME).a
	@echo Combined \
    `find ./build_dir/iphonesimulator -type f -name '*.a' 2>/dev/null | grep -v 'iphonesimulator/lib'` \
    `find ./build_dir/iphoneos -type f -name '*.a' 2>/dev/null | grep -v 'iphoneos/lib'` \
    `find ./build_dir/macos -type f -name '*.a' 2>/dev/null | grep -v 'macos/lib'` \
		into ./build_dir/universal/lib$(LIBNAME).a

veryclean:
	rm -fR "$(BUILD_DIR)"

dirs: $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p "$@"
