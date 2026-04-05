export PATH_TO_NDK=/opt/android-ndk-r21e

ENGINE_SRC="./android/pet_detector/../../engine/src"
touch $ENGINE_SRC/utils/*.hpp
touch $ENGINE_SRC/pdt_impl.cpp
touch ./android/pet_detector/src/jni/ObjectDetectorJNI.cpp
touch ./android/pet_detector/src/jni/jni_wrapper_utils.cpp

cd ./android/pet_detector/src/jni
# export PATH_TO_NDK=~/Android/android-ndk-r20
$PATH_TO_NDK/ndk-build -j16
cd -
mkdir -p ./android/pet_detector/libs
cp -r ./android/pet_detector/src/libs/* ./android/pet_detector/libs/

exit $?
