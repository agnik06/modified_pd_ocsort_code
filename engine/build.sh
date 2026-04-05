RED='\033[0;31m'
LightGreen='\033[1;32m'
Yellow='\033[1;33m'
NC='\033[0m' # No Color

mkdir -p ./build/x86
cd ./build/x86
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
cmake ../..
make install -j8

#printf "\n${LightGreen}Build Completed ! Output directory: ./libs \n${NC}"
