export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:third_party/DLInterface/libs/x86_64
echo "~""$*""~"
if [ "$*" != "" ] 
then
	./bin/x86_64/PetDetector_v1.camera.samsung "$*" #"$@" $@ "$*" $*
else
	./bin/x86_64/PetDetector_v1.camera.samsung
fi
