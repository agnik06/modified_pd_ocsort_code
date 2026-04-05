ENGINE_SRC="../../engine/src"
touch $ENGINE_SRC/utils/*.hpp
touch $ENGINE_SRC/object_detector_impl.cpp
touch src/jni/ObjectDetectorJNI.cpp
touch src/jni/jni_wrapper_utils.cpp
cd src/jni
# export PATH_TO_NDK=~/Android/android-ndk-r20
$PATH_TO_NDK/ndk-build -j16
cd -
mkdir -p libs_hwasan
cp -r src/libs/* libs_hwasan/

#Changes for HWASAN build from here
#mkdir -p ./libs_HWASAN

#cp -r ./libs/arm64-v8a ./libs_HWASAN/

#rm -rf src/libs/arm64-v8a/libLightObjectDetectorJNI_HWASAN.so ./libs/arm64-v8a/libLightObjectDetector_v1_HWASAN.camera.samsung.so
#rm -rf ./libs/arm64-v8a/libLightObjectDetectorJNI_HWASAN.so ./libs/arm64-v8a/libLightObjectDetector_v1_HWASAN.camera.samsung.so
#mv ./libs_HWASAN/arm64-v8a/libLightObjectDetector_v1_HWASAN.camera.samsung.so ./libs_HWASAN/arm64-v8a/libLightObjectDetector_v1.camera.samsung.so
#mv ./libs_HWASAN/arm64-v8a/libLightObjectDetectorJNI_HWASAN.so ./libs_HWASAN/arm64-v8a/libLightObjectDetectorJNI.so
#rm -rf ./libs_HWASAN/arm64-v8a/libc++.so ./libs_HWASAN/arm64-v8a/libOpenCv*

#mv ./libs_HWASAN ./output_libs/


exit $?
