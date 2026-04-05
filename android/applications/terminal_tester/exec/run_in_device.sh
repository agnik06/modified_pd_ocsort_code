export PATH=$PATH:$HOME"/Android/Sdk/platform-tools"
echo $PWD
adb -s R3CN30P1KWZ push ObjectDetectorTester /system/ObjectDetectorTester
adb -s R3CN30P1KWZ shell chmod a+x /system/ObjectDetectorTester
adb -s R3CN30P1KWZ shell /system/ObjectDetectorTester
