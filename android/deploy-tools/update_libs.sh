libs_type=arm64-v8a
source variables.sh

echo "Updating libs in Android"

#libs_type=armeabi-v7a


echo $APP_DIR/vz_object_detector/src/main/jniLibs/
APP_DIR_LIBS=$APP_DIR/vz_object_detector/src/main/jniLibs/$libs_type	
mkdir -p $APP_DIR_LIBS	
rm $APP_DIR_LIBS/*
for lib_path in "${DEPLOY_LIBS[@]}"
do
	ls $lib_path -lh
	cp $lib_path $APP_DIR_LIBS/		
done
