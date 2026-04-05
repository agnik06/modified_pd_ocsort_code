This file contains the necessary instructions to build the generic object detection module

Build instructions:
git checkout develop
(develop is the branch where the current development happens. But it can be any branch)
cd android/deploy_tools/
export PATH_TO_NDK = <ndk-path with ctc++ enabled>
comment APP_ABI+=armeabi-v7a to disable 32-bit build in android/object_detector/src/jni/Application.mk
./build_libs.sh
./update_libs.sh  #make sure libSceneDetectorJNI.so,libObject_detector_v1.camera.samsung.so and libhota.so are present in app's jnilibs folder order to get logs for codecoverage testing. Same apk and lib must NOT be used in release. release lib must be built with ctc disabled.
./update_apk.sh


Test Instructions:

*Run the app on images with basic test cases covered
*capture the logs after execution
run fill2ctcdata and report-Generation command to generate reports.
