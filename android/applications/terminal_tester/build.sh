export PATH_TO_NDK=~/Android/android-ndk-r21e
cd src/jni
$PATH_TO_NDK/ndk-build -j16
cd -
mkdir -p exec
cp -r src/libs/arm64-v8a/LightObjectDetectorTester exec/
exit $?
