source variables.sh
source update_apk.sh

$ADB install -r apps/$APP_NAME.apk
#$ADB shell pm install -r "/sdcard/Apps/"$APP_NAME.apk
#adb shell am start -n "vizinsight.atl.maskon/vizinsight.atl.maskon.MainActivity" -a android.intent.action.MAIN -c android.intent.category.LAUNCHER
