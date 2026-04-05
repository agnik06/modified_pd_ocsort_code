#APP_STL := c++_static
APP_STL := none
#APP_CPPFLAGS := -frtti -fexceptions -std=c++11 -Wno-return-type
#APP_STL := none
#APP_CPPFLAGS := -frtti -fexceptions -std=c++11 -Wno-return-type
#APP_CPPFLAGS := -frtti -fexceptions -std=c++14 -Wno-return-type -Wno-comment -Wno-switch
APP_CPPFLAGS := -frtti -fexceptions -std=c++17 -Wno-return-type -nostdlib++ 

APP_ABI := arm64-v8a
#APP_ABI := armeabi-v7a-hard
APP_ABI += armeabi-v7a
APP_PLATFORM := android-27
NDK_PLATFORM := android-27
#NDK_TOOLCHAIN_VERSION := 4.9
NDK_TOOLCHAIN_VERSION := clang