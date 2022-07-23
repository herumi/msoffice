########################################################################
# arch-derived.mk ABr
# Derived variables for a specific architecture

ifeq ($(ARCH),)
$(error Must define ARCH to invoke this Makefile)
endif

# build output for this particular architecture
ARCH_BUILD_DIR:=$(BUILD_DIR)/$(ARCH)
ARCH_OBJ_DIR:=$(ARCH_BUILD_DIR)/obj
ARCH_LIB_DIR:=$(ARCH_BUILD_DIR)/lib
ARCH_LIB_PATH:=$(ARCH_LIB_DIR)/lib$(LIBNAME).a
ARCH_SO_PATH:=$(ARCH_LIB_DIR)/lib$(LIBNAME).so

# static settings (compiling only on mac)
the_os_type:=linux
the_build_platform:=android
the_cc_platform_defines:=-DANDROID -D__ANDROID_API__=$(ANDROID_API) 

# includes for msoc
INCLUDES:=-I$(OPENSSL_DIR)/$(ARCH)/include -I$(MSOC_DIR)/include -I$(TOP_DIR)/cybozulib/include -I$(SIMDE_DIR)
LIBS:=-L$(OPENSSL_DIR)/$(ARCH) -lcrypto

# analyze the architecture
ifeq ($(ARCH),armeabi-v7a)
the_arch:=arm
the_abi:=$(ARCH)
the_arch_suffix:=eabi
the_cc_prefix:=armv7a
the_tool_prefix:=arm
the_cc_arch:=armv7-a
the_cc_opts_extra:=-DARM32 -mfpu=vfpv3-d16 -mfloat-abi=softfp 
the_ld_opts_extra:=-mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8
endif
ifeq ($(ARCH),arm64-v8a)
the_arch:=arm64
the_abi:=$(ARCH)
the_arch_suffix:=
the_cc_prefix:=aarch64
the_tool_prefix:=$(the_cc_prefix)
the_cc_arch:=armv8-a
the_cc_opts_extra:=-DARM64
endif
ifeq ($(ARCH),x86)
the_arch:=$(ARCH)
the_abi:=$(ARCH)
the_arch_suffix:=
the_cc_prefix:=i686
the_tool_prefix:=$(the_cc_prefix)
the_cc_arch:=$(the_cc_prefix)
the_cc_opts_extra:=-mtune=intel -mssse3 -mfpmath=sse -m32
endif
ifeq ($(ARCH),x86_64)
the_arch:=$(ARCH)
the_abi:=$(ARCH)
the_arch_suffix:=
the_cc_prefix:=x86_64
the_tool_prefix:=$(the_cc_prefix)
the_cc_arch:=x86-64
the_cc_opts_extra:=-msse4.2 -mpopcnt -m64 -mtune=intel
endif

# set derived variables
the_abi_triple:=$(the_arch)-$(the_os_type)-$(the_build_platform)

# set compile tools
AR:=$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)-ar
CC:=$(the_cc_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)$(ANDROID_API)-clang
CXX:=$(the_cc_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)$(ANDROID_API)-clang++
AS:=$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)-as
LD:=$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)-ld
RANLIB:=$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)-ranlib
STRIP:=$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)-strip

# location of libc++_shared.so - required for msoc
TOOLCHAIN_SHARED_LIBS_DIR:=$(XAMARIN_ANDROID_NDK_SYSROOT_DIR)/usr/lib/$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)
TOOLCHAIN_SHARED_LIB_CXX_NAME:=libc++_shared.so
TOOLCHAIN_SHARED_LIB_CXX=$(TOOLCHAIN_SHARED_LIBS_DIR)/$(TOOLCHAIN_SHARED_LIB_CXX_NAME)

# flags
ARFLAGS:=rs
CFLAGS:=-march=$(the_cc_arch) $(the_cc_opts_extra) -Wno-unused-function -fno-integrated-as -fstrict-aliasing -fPIC $(the_cc_platform_defines) -Os -ffunction-sections -fdata-sections -I$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include -I$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/local/include $(INCLUDES)
CPPFLAGS:=-march=$(the_cc_arch) $(the_cc_opts_extra) -Wno-unused-function -fno-integrated-as -fstrict-aliasing -fPIC $(the_cc_platform_defines) -Os -ffunction-sections -fdata-sections -I$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include -I$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/local/include $(INCLUDES)
CXXFLAGS:=-std=c++14 -Os -ffunction-sections -fdata-sections
LDFLAGS:=-march=$(the_cc_arch) $(the_ld_opts_extra) -Wl,--gc-sections -Os -ffunction-sections -fdata-sections -L$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)/lib -L$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/lib/$(the_tool_prefix)-$(the_os_type)-$(the_build_platform)$(the_arch_suffix)/$(ANDROID_API) -L$(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/darwin-x86_64/lib $(LIBS) -latomic -lc++ -Wl,-znodelete -shared -Wl,-Bsymbolic
#-static-libstdc++
#-Wno-unused-command-line-argument

#/home/user/Android/Sdk/ndk/21.1.6352462/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ \
#	--target=armv7-none-linux-androideabi24 \
#	--gcc-toolchain=/home/user/Android/Sdk/ndk/21.1.6352462/toolchains/llvm/prebuilt/linux-x86_64 \
#	--sysroot=/home/user/Android/Sdk/ndk/21.1.6352462/toolchains/llvm/prebuilt/linux-x86_64/sysroot \
#	-fPIC -g -DANDROID -fdata-sections -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes \
#	-D_FORTIFY_SOURCE=2 -march=armv7-a -mthumb -Wformat -Werror=format-security  -std=c++17 -O0 -fno-limit-debug-info  \
#	-Wl,--exclude-libs,libgcc.a -Wl,--exclude-libs,libgcc_real.a -Wl,--exclude-libs,libatomic.a \
#	-static-libstdc++ \
#	-Wl,--build-id -Wl,--fatal-warnings -Wl,--exclude-libs,libunwind.a -Wl,--no-undefined \
#	-Qunused-arguments \
#	-shared \
#	-Wl,-soname,liborwell_android.so \
#	-o /home/user/orwell2/orwell_flutter_app/build/app/intermediates/cmake/debug/obj/armeabi-v7a/liborwell_android.so \
#	CMakeFiles/orwell_android.dir/orwell_jni.cpp.o \
#	CMakeFiles/orwell_android.dir/DecodedFfmpegFrameJNI.cpp.o \
#	CMakeFiles/orwell_android.dir/JavaOrwellFlutterRenderer.cpp.o \
#	CMakeFiles/orwell_android.dir/MediaCodecDecoder.cpp.o \
#	CMakeFiles/orwell_android.dir/JavaSimpleFileWriter.cpp.o \
#	CMakeFiles/orwell_android.dir/JavaHashMapJNI.cpp.o \
#	CMakeFiles/orwell_android.dir/JavaFlutterEventMessenger.cpp.o  \
#	_liborwell/liborwell_static.a \
#	-landroid \
#	/home/user/Android/Sdk/ndk/21.1.6352462/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/arm-linux-androideabi/24/liblog.so \
#	-lmediandk \
#	/home/user/orwell2/orwell_flutter_app/android/app/src/main/cpp/../../../../../../deps/ffmpeg/build/android/armeabi-v7a/lib/libavcodec.so \
#	/home/user/orwell2/orwell_flutter_app/android/app/src/main/cpp/../../../../../../deps/ffmpeg/build/android/armeabi-v7a/lib/libavutil.so \
#	/home/user/orwell2/orwell_flutter_app/android/app/src/main/cpp/../../../../../../deps/ffmpeg/build/android/armeabi-v7a/lib/libswscale.so \
#	/home/user/orwell2/orwell_flutter_app/android/app/src/main/cpp/../../../../../../deps/ffmpeg/build/android/armeabi-v7a/lib/libswresample.so \
#	-llog \
#	/home/user/orwell2/orwell_flutter_app/build/app/intermediates/cmake/debug/obj/armeabi-v7a/libmyRtspClient.so \
#	/home/user/orwell2/orwell_flutter_app/build/app/intermediates/cmake/debug/obj/armeabi-v7a/libjrtp.so \
#	_liborwell/_myRtspClient/libmyRtspClient-static.a \
#	_liborwell/_myRtspClient/_JTRPLIB/src/libjrtp.a \
#	_liborwell/_ZLMediaKit/libzlmediakit.a \
#	_liborwell/_ZLMediaKit/libzltoolkit.a \
#	_liborwell/_ZLMediaKit/libmpeg.a \
#	_liborwell/_ZLMediaKit/libmov.a \
#	_liborwell/_ZLMediaKit/libflv.a \
#	_liborwell/common/openvpn_zl_socket/libopenvpn_zl_socket.a \
#	_liborwell/common/openvpn_myrtsp_socket/libmyrtsp_vpn_tcp_transmitter.a \
#	_liborwell/_libopenvpn3/src/libopenvpn/libopenvpn_lib.a \
#	_liborwell/_libopenvpn3/openvpn3/libssl.a \
#	_liborwell/_libopenvpn3/openvpn3/libcrypto.a \
#	-lpthread \
#	_liborwell/_libopenvpn3/openvpn3/liblzo.a \
#	_liborwell/_libopenvpn3/openvpn3/liblz4.a \
#	_liborwell/_libopenvpn3/libtins/lib/libtins.a \
#	_liborwell/_libopenvpn3/smoltcp_cpp_interface/libsmoltcp_cpp_static.a \
#	/home/user/orwell2/deps/libopenvpn3/smoltcp_cpp_interface/target/debug/libsmoltcp_cpp_interface_rust.a \
#	-ldl \
#	_liborwell/common/liborwellebml/liborwell_ebml.a \
#	_liborwell/_libebml/libebml.a \
#	_liborwell/common/liborwellprofile/liborwell_profile.a \
#	-latomic \
#	-lm && :


# list of source files and objects (base names only)
SRCS:=$(shell find $(SRC_DIR) -type f -name 'msocdll.cpp' -maxdepth 1 -exec basename {} \;)
OBJS:=$(SRCS:.cpp=.o)

# actual objects for this architecture
ARCH_OBJS:=$(foreach obj,$(OBJS),$(ARCH_OBJ_DIR)/$(obj))
