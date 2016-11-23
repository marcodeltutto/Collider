#!/bin/bash
set -e
export GCC_THUMB_SUPPORT='NO'
PATH="/Applications/Marmalade.app/Contents/s3e/bin":$PATH
PATH="/Applications/Marmalade.app/Contents/s3e/gcc/darwin/aarch64/bin":$PATH
PATH="/Applications/Marmalade.app/Contents/s3e/gcc/darwin/arm/bin":$PATH
PATH="/Applications/Marmalade.app/Contents/tools/gcc/arm/android_osx/bin":$PATH
PATH="/Applications/Marmalade.app/Contents/tools/gcc/x86/android_osx/bin":$PATH
PATH="/usr/bin":$PATH
PATH="/Applications/Xcode.app/Contents/Developer/usr/bin":$PATH
PATH="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin":$PATH
PATH="/opt/local/bin":$PATH
cd "/Applications/Marmalade.app/Contents/s3e/tools"
/System/Library/Frameworks/Python.framework/Versions/2.7/Resources/Python.app/Contents/MacOS/Python ./process_map.py "${TARGET_BUILD_DIR}/${EXECUTABLE_NAME}"
