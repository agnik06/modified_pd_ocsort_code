source variables.sh

function release_libs(){
	libs_type=$1
	RELEASE_DIR_LIBS=$RELEASE_DIR/Libs/$libs_type
	mkdir -p $RELEASE_DIR_LIBS
	
	source variables.sh
	for lib_path in "${RELEASE_LIBS[@]}"
	do
		echo $lib_path
		cp $lib_path $RELEASE_DIR_LIBS/		
	done
}

function create_release_package(){
	#MODELS
	#echo ================Updating Models================
	#mkdir -p $RELEASE_DIR/Models/
	#cp -r $MODELS_DIR/* $RELEASE_DIR/Models/
	#find $RELEASE_DIR/Models/*

	#LIBS
	echo ================Updating Libs==================
	mkdir -p $RELEASE_DIR/Libs/

	for arch in "${ARCH[@]}"
	do
		release_libs $arch
	done
	#release_libs arm64-v8a 
	#release_libs armeabi-v7a
	#release_libs x86_64 
	find $RELEASE_DIR/Libs/*

	#APP
	echo ================Updating Apps==================

	mkdir -p $RELEASE_DIR/App/

	./update_libs.sh | grep @#@#


	CURR_DIR=${PWD}
	cd $APP_DIR
	bash gradlew clean | grep @#@#
	bash gradlew assembleDebug | grep @#@#
	cd $CURR_DIR

    cp $APP_DIR/app/build/outputs/apk/app-debug.apk $RELEASE_DIR/App/$APP_NAME.apk
    cp $APP_DIR/app/build/outputs/apk/debug/app-debug.apk $RELEASE_DIR/App/$APP_NAME.apk

	cp $APP_DIR/image_segmenter/build/outputs/aar/image_segmenter-debug.aar $RELEASE_DIR/App/
	cp $APP_DIR/image_segmenter/build/outputs/aar/image_segmenter-release.aar $RELEASE_DIR/App/
	find $RELEASE_DIR/apps/*
}

function release_libs_hwasan(){
	source variables.sh
	libs_type=$1
	RELEASE_DIR_HWASAN=./releases/pet-detector
	mkdir -p $RELEASE_DIR_HWASAN/Libs_hwasan/
	RELEASE_DIR_LIBS=$RELEASE_DIR_HWASAN/Libs_hwasan/$libs_type

	mkdir -p $RELEASE_DIR_LIBS
	
	echo ================Updating release_libs_hwasan ==================
	for lib_path in "${RELEASE_LIBS_HWASAN[@]}"
	do
		echo $lib_path
		cp -r $lib_path $RELEASE_DIR_LIBS/		
	done
}

function create_release_package_hwasan(){
	#MODELS
	#echo ================Updating HWASAN libs================
	#mkdir -p $RELEASE_DIR/Models/
	#cp -r $MODELS_DIR/* $RELEASE_DIR/Models/
	#find $RELEASE_DIR/Models/*

	#LIBS
	echo ================Updating HWASAN Libs==================
	#source variables.sh

	for arch in "${ARCH[@]}"
	do
		release_libs_hwasan $arch
	done
	#release_libs arm64-v8a 
	#release_libs armeabi-v7a
	#release_libs x86_64 
	find $RELEASE_DIR_HWASAN/Libs_hwasan/*

	#APP
	echo ================Updating Apps==================

	#mkdir -p $RELEASE_DIR/App/

	#./update_libs.sh | grep @#@#


	#CURR_DIR=${PWD}
	#cd $APP_DIR
	#bash gradlew clean | grep @#@#
	#bash gradlew assembleDebug | grep @#@#
	#cd $CURR_DIR

    #cp $APP_DIR/app/build/outputs/apk/app-debug.apk $RELEASE_DIR/App/$APP_NAME.apk
    #cp $APP_DIR/app/build/outputs/apk/debug/app-debug.apk $RELEASE_DIR/App/$APP_NAME.apk

	#cp $APP_DIR/image_segmenter/build/outputs/aar/image_segmenter-debug.aar $RELEASE_DIR/App/
	#cp $APP_DIR/image_segmenter/build/outputs/aar/image_segmenter-release.aar $RELEASE_DIR/App/
	#find $RELEASE_DIR/apps/*
}

rm -r $RELEASE_DIR
#Test version
CURR_DIR=${PWD}
cd $ENGINE_DIR
./build.sh
cd $CURR_DIR
create_release_package

cp $ENGINE_DIR/../../engine/include/object_detector/* $RELEASE_DIR/Libs/
echo ================Building HWASAN ==================
echo $ENGINE_DIR
#hwasan
#sed -i -e 's/# LOCAL_CFLAGS += -DLIBBUILDMODE='"hwasan"'/LOCAL_CFLAGS += -DLIBBUILDMODE='"hwasan"'/g' $ENGINE_DIR/src/jni/Android.mk
sed -i -e 's/# LOCAL_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer/LOCAL_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer/g' $ENGINE_DIR/src/jni/Android.mk
sed -i -e 's/# LOCAL_LDFLAGS += -fsanitize=hwaddress/LOCAL_LDFLAGS += -fsanitize=hwaddress/g' $ENGINE_DIR/src/jni/Android.mk

CURR_DIR=${PWD}
cd $ENGINE_DIR
./build_hwasan.sh
cd $CURR_DIR
#create_release_package_hwasan

echo ================Moving Libs to $RELEASE_DIR/Libs_HWASAN ==================
mv $ENGINE_DIR/libs_hwasan $RELEASE_DIR/


#sed -i -e 's/LOCAL_CFLAGS += -DLIBBUILDMODE='"hwasan"'/# LOCAL_CFLAGS += -DLIBBUILDMODE='"hwasan"'/g' $ENGINE_DIR/src/jni/Android.mk
sed -i -e 's/LOCAL_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer/# LOCAL_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer/g' $ENGINE_DIR/src/jni/Android.mk
sed -i -e 's/LOCAL_LDFLAGS += -fsanitize=hwaddress/# LOCAL_LDFLAGS += -fsanitize=hwaddress/g' $ENGINE_DIR/src/jni/Android.mk
echo ================Moving Libs to $RELEASE_DIR/Libs ==================
mv $ENGINE_DIR/libs $RELEASE_DIR/
