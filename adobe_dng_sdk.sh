#!/bin/bash
set -e
set -u
set -o pipefail

workdir="sdk"

ver_xlib="1.3"
ver_expat="2.1.0"

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 [build|install]"
  echo ""
  echo "build: Download and build the Adobe DNG 1.4 sdk"
  echo "install: Install the dng_validate command and required libraries (optional)"
  exit 0
fi

if [ "$1" == "install" ]; then

  if [ -f "$workdir"/dng_validate ]; then
    install -Dm755 "$workdir"/dng_validate /usr/local/bin/dng_validate
    install -Dm644 "$workdir"/libXMPCore.so /usr/local/lib/libXMPCore.so
    install -Dm644 "$workdir"/libXMPFiles.so /usr/local/lib/libXMPFiles.so
    exit 0
  else
    echo "Build the SDK first with the $0 build command (without superuser privilegs)"
    exit 1
  fi
fi

if [ "$1" != "build" ]; then
  echo "Invalid command"
  exit 1
fi

mkdir -p "$workdir"
workdir=$(readlink -f "$workdir")
echo "Working in '$workdir'"
cd "$workdir"

# Get XMP
if [ -d "xmp_sdk" ]; then
  echo "> XMP sdk was already downloaded"
else
  echo "> Downloading XMP"
  wget "http://download.macromedia.com/pub/developer/xmp/sdk/XMP-Toolkit-SDK-CC-201306.zip"
  unzip XMP-Toolkit-SDK-CC-201306.zip
  mv XMP-Toolkit-SDK-CC201306 xmp_sdk
  rm -f XMP-Toolkit-SDK-CC-201306.zip
fi

xmp_root="$workdir"/xmp_sdk

# Get DNG sdk
if [ -d "dng_sdk_1_4" ]; then
  echo "> DNG sdk was already downloaded"
else
  echo "> Downloading DNG sdk"
  wget "http://download.adobe.com/pub/adobe/dng/dng_sdk_1_4.zip"
  unzip dng_sdk_1_4.zip
  rm -f dng_sdk_1_4.zip
fi

dng_root="$workdir"/dng_sdk_1_4

# Get zlib
cd "$xmp_root"/third-party/zlib
if [ -f "zlib.h" ]; then
  echo "> ZLIB was already downloaded"
else
  echo "> Downloading ZLIB $ver_zlib"
  wget "http://zlib.net/zlib-$ver_zlib.tar.gz"
  tar xzf zlib-$ver_zlib.tar.gz
  cp zlib-$ver_zlib/*.h zlib-$ver_zlib/*.c .
fi

# Get expat
cd "$xmp_root"/third-party/expat
if [ -f "lib/expat.h" ]; then
  echo "> EXPAT was already downloaded"
else
  echo "> Downloading EXPAT $ver_expat"
  wget "https://downloads.sourceforge.net/project/expat/expat/$ver_expat/expat-$ver_expat-RENAMED-VULNERABLE-PLEASE-USE-2.3.0-INSTEAD.tar.gz"
  mv expat-$ver_expat-RENAMED-VULNERABLE-PLEASE-USE-2.3.0-INSTEAD.tar.gz expat-$ver_expat.tar.gz
  tar xzf expat-$ver_expat.tar.gz
  cp -R expat-$ver_expat/lib .
fi

# Build XMP
cd "$xmp_root"
if [ -f "public/libraries/i80386linux_x64/release/libXMPCore.so" ]; then
  echo "> XMP was already built"
else
  echo "> Building XMP"
  cd "$xmp_root"/build
  sed -i '/set(XMP_ENABLE_SECURE_SETTINGS "ON")/c\set(XMP_ENABLE_SECURE_SETTINGS "OFF")' shared/ToolchainGCC.cmake
  sed -i '15 i #include "string.h"' ../XMPFiles/source/NativeMetadataSupport/ValueObject.h
  make
fi

xmp_build="$xmp_root"/public/libraries/i80386linux_x64/release

# Build DNG sdk
cd "$dng_root"
cd dng_sdk/source

if [ -f "dng_validate" ]; then
  echo "> DNG sdk was already built"
else
  cat > Makefile <<'EOF'
# Binary name
APP=dng_validate

INCL=-I $(XMP_PUB_DIR)/include
XMP_RELEASE=$(XMP_PUB_DIR)/libraries/i80386linux_x64/release
LIB=-ljpeg -lz -lpthread -ldl -L $(XMP_RELEASE) -lXMPCore -lXMPFiles

# Execute the binary
all: $(APP)

# Linking
$(APP): *.o
	g++ $^ $(LIB) -Wl,-rpath=. -Wl,-rpath=/usr/local/lib -o $(APP)

# Compilation
*.o: *.cpp
	g++ -c -Wall $(INCL) -DqDNGValidateTarget=1 $^

clean:
	rm *.o
EOF

  cat > RawEnvironment.h << 'EOF'
#define qLinux 1
#define qDNGThreadSafe 1
#define UNIX_ENV 1
EOF

  cat > "$workdir"/sdk.patch << 'EOF'
diff --git a/dng_sdk/source/dng_string.cpp b/dng_sdk/source/dng_string.cpp
index 6eafae1..82be01b 100644
--- a/dng_sdk/source/dng_string.cpp
+++ b/dng_sdk/source/dng_string.cpp
@@ -30,7 +30,7 @@
 #include <windows.h>
 #endif

-#if qiPhone || qAndroid
+#if qiPhone || qAndroid || qLinux
 #include <ctype.h> // for isdigit
 #endif

diff --git a/dng_sdk/source/dng_xmp_sdk.cpp b/dng_sdk/source/dng_xmp_sdk.cpp
index 77d7ebe..3231587 100644
--- a/dng_sdk/source/dng_xmp_sdk.cpp
+++ b/dng_sdk/source/dng_xmp_sdk.cpp
@@ -47,7 +47,7 @@

 #define XMP_INCLUDE_XMPFILES qDNGXMPFiles

-#define XMP_StaticBuild 1
+#define XMP_StaticBuild 0

 #if qiPhone
 #undef UNIX_ENV
EOF

  patch -ruN -d "$dng_root" --strip 1 < "$workdir"/sdk.patch
  rm -f "$workdir"/sdk.patch

  make XMP_PUB_DIR="$xmp_root"/public
fi

cp "$dng_root"/dng_sdk/source/dng_validate "$workdir"/dng_validate
cp "$xmp_build"/{libXMPCore.so,libXMPFiles.so} "$workdir"/