echo "~"$*"~"
export PATH=$PATH:$HOME"/Android/Sdk/platform-tools"
#Get device id & config path
device_id="R3CN30P1KWZ"
while getopts s:c: option #not 'p:' as p doesn't need value
do
 case "${option}"
 in
 s) device_id=${OPTARG};;
 c) config_path=${OPTARG};;
 esac
done

if [ "$device_id" == "" ]; then
	ADB=adb
else
    echo "Using Device: $device_id"
	ADB="adb -s $device_id"
fi

ADB="adb -s $device_id"
# Push the setup
$ADB wait-for-device
$ADB root
$ADB remount
$ADB shell setenforce 0

$ADB shell "mkdir -p /system/usr/g_od"
$ADB push exec/ObjectDetectorTester /system/usr/g_od/ObjectDetectorTester
$ADB shell "chmod a+x /system/usr/g_od/ObjectDetectorTester"

$ADB push test_image.jpg /system/usr/g_od/test_image.jpg

# $ADB shell "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/system/usr/g_od"

# Run the setup
$ADB shell "./system/usr/g_od/ObjectDetectorTester 10"
