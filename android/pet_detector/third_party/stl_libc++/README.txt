The headers are extracted from Android P P4 repository.
The path of them are :
    //PEACE/Strawberry/EXYNOS5/android/external/libcxx/include
    //PEACE/Strawberry/EXYNOS5/android/external/libcxxabi/include
I checked the other path for Qualcomm but the headers are identical.

For the libc++.so is extracted from the device's /system/lib[64].

libc++.so is in the P4 repository but there are some missing symbols.
I guess the libc++.so on the device is built with libc++.a and libc++abi.so in P4 repository.
The size of on-device shared lib is bigger than repository's one.

For now, only with android-ndk-r16b it was tested.
