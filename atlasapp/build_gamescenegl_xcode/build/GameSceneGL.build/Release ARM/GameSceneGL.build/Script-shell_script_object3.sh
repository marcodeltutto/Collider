#!/bin/bash
set -e
if [ "$CONFIGURATION" = "Release" -o "$CONFIGURATION" = "Release ARM" -o "$CONFIGURATION" = "Release AARCH64" -o "$CONFIGURATION" = "Release X86_64 NaCL" ]; then echo "skipping for release builds" && exit 0; fi
if [ "$CONFIGURATION" = "Debug ARM" -o "$CONFIGURATION" = "Release ARM" -o "$CONFIGURATION" = "Debug AARCH64" -o "$CONFIGURATION" = "Release AARCH64" -o "$CONFIGURATION" = "Debug X86_64 NaCL" -o "$CONFIGURATION" = "Release X86_64 NaCL" ]; then echo "x86: skipping for arm and aarch64 and NaCl builds" && exit 0; fi
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
s3e_plink --license "/Users/deltutto/My_Collider/atlasapp/build_gamescenegl_xcode/build/${CONFIGURATION}${EFFECTIVE_PLATFORM_NAME}/GameSceneGL.s86" --buildenv=XCODE_8.1 --arch=x86 --config=debug
