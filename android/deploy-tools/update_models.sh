source variables.sh

model_dir=$MODELS_DIR
remove_old_models=false
while getopts i:s:d option #not 'p:' as p doesn't need value
do
 case "${option}"
 in
 i) model_dir=${OPTARG};;
 d) remove_old_models=true;;
 esac
done

$ADB shell 'mount -o rw,remount /system'	
$ADB shell 'mkdir -p '$DEVICE_MODELS_DIR
if $remove_old_models ; then
	echo "deleting old models"
	$ADB shell rm -r /sdcard/saiv/image_understanding/db/image_segmentation
fi
$ADB push $model_dir/* $DEVICE_MODELS_DIR

