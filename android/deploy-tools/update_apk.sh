source variables.sh

echo "Updating app : "$APP_DIR
CURR_DIR=${PWD}
cd $APP_DIR

clean_build=false
while getopts c option #not 'p:' as p doesn't need value
do
 case "${option}"
 in
 c) clean_build=true;;
 esac
done

if $clean_build ; then
    bash gradlew clean
fi 

bash gradlew assembleDebug --stacktrace
cd $CURR_DIR

cp $APP_DIR/app/build/outputs/apk/app-debug.apk App/$APP_NAME.apk
cp $APP_DIR/app/build/outputs/apk/debug/app-debug.apk App/$APP_NAME.apk

echo "APK file created in "${PWD}/apps/$APP_NAME.apk
