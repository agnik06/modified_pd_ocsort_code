MODELS_DIR=models/light_object_detector
DEVICE_MODELS_DIR=/sdcard/saiv/image_understanding/db
#DEVICE_MODELS_DIR=/vendor/saiv/image_understanding/db/image_segmentation

ENGINE_DIR=../pet_detector

APP_DIR=../applications/BoxOn
APP_NAME=VideoAutoFocus

#APP_DIR=../image-segmentation/android/applications/SceneOptimizerPro
#APP_NAME=SceneOptimizerPro

RELEASE_DIR=./releases/pet-detector

ARCH=(
	arm64-v8a 
	armeabi-v7a
	#x86_64 
)

export PATH_TO_ADB=~/Android/Sdk/platform-tools
export ANDROID_HOME=~/Android/Sdk/

OUTPUT_LIBS_DIR=$ENGINE_DIR/libs
OUTPUT_LIBS_DIR_HWASAN=$ENGINE_DIR/libs_hwasan
DLI_LIBS_DIR=../../engine/third_party/DLInterface/libs
DEPLOY_LIBS=(
	$OUTPUT_LIBS_DIR/$libs_type/libOpenCv.camera.samsung.so
	$OUTPUT_LIBS_DIR/$libs_type/libOpenCv_clang.so
	$OUTPUT_LIBS_DIR/$libs_type/libc++.so
	$OUTPUT_LIBS_DIR/$libs_type/libPetDetector_v1.camera.samsung.so
	$OUTPUT_LIBS_DIR/$libs_type/libPetDetectorJNI.so	
	$DLI_LIBS_DIR/$libs_type/libDLInterface_hidl.camera.samsung.so	
)

RELEASE_LIBS=(
	$OUTPUT_LIBS_DIR/$libs_type/libPetDetector_v1.camera.samsung.so
)

DEPLOY_LIBS_HWASAN=(
	$OUTPUT_LIBS_DIR/$libs_type/libpPetDetector_v1.camera.samsung.so
	$OUTPUT_LIBS_DIR/$libs_type/libPetDetectorJNI.so
)

RELEASE_LIBS_HWASAN=(
	$OUTPUT_LIBS_DIR_HWASAN/$libs_type/libPetDetector_v1.camera.samsung.so
)

time_stamp=$(date +%Y-%m-%d-%T)
RED='\033[0;31m'
NC='\033[0m' 
BLUE='\033[0;34m'

#Get device id
device_id=""
while getopts i:s:d option #not 'p:' as p doesn't need value
do
 case "${option}"
 in
 s) device_id=${OPTARG};;
 esac
done

if [ "$device_id" == "" ]; then
	ADB=adb
else
    echo "Using Device: $device_id"
	ADB="adb -s $device_id"
fi

mkdir -p App
mkdir -p Models
mkdir -p Libs
mkdir -p releases
