LOCAL_PATH := $(call my-dir)
ENGINE_BASE_PATH 				:= $(LOCAL_PATH)/../../../../engine
ENGINE_SRC_PATH 				:= $(ENGINE_BASE_PATH)/src
ENGINE_THIRD_PARTY_LIBS_PATH 	:= $(ENGINE_BASE_PATH)/third_party
THIRD_PARTY_LIBS_PATH 			:= $(LOCAL_PATH)/../../third_party

#INCLUDE PATHS
OPENCV_INC_PATH 		:= $(THIRD_PARTY_LIBS_PATH)/pcv340/include
STL_INCLUDE 			:= $(THIRD_PARTY_LIBS_PATH)/stl_libc++/include/libcxx_inc
JSON_CPP_INCLUDE 		:= $(ENGINE_THIRD_PARTY_LIBS_PATH)/json
DL_INTERFACE_INCLUDE 	:= $(ENGINE_THIRD_PARTY_LIBS_PATH)/DLInterface/include/
EIGEN_CPP_INCLUDE       := $(ENGINE_THIRD_PARTY_LIBS_PATH)/Eigen/include
OBJECT_DETECTOR_INCLUDE := $(ENGINE_BASE_PATH)/include/object_detector
ENGINE_CORE_INCLUDE 	:= $(ENGINE_SRC_PATH)/core
ENGINE_SRC_INCLUDE	 	:= $(ENGINE_SRC_PATH)/
ENGINE_TRACKER_INCLUDE  := $(ENGINE_SRC_PATH)/modules/detector/tracker_wrapper/tracker
#------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE := OPENCV_HQ
LOCAL_LDLIBS := -llog
LOCAL_SRC_FILES := $(THIRD_PARTY_LIBS_PATH)/pcv340/libs/$(TARGET_ARCH_ABI)/libOpenCv.camera.samsung.so
# LOCAL_SRC_FILES := $(THIRD_PARTY_LIBS_PATH)/opencv340/libs/$(TARGET_ARCH_ABI)/libOpenCv_clang.so
include $(PREBUILT_SHARED_LIBRARY)

#------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE := STL_LIBC++
LOCAL_LDLIBS := -llog
LOCAL_SRC_FILES := $(THIRD_PARTY_LIBS_PATH)/stl_libc++/libs/$(TARGET_ARCH_ABI)/libc++.so
include $(PREBUILT_SHARED_LIBRARY)

#------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE := PetDetector_v1.camera.samsung
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -DLIBBUILDMODE='"default"'
LOCAL_CFLAGS += -fvisibility=hidden 

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	LOCAL_CFLAGS += -march=armv8-a
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_SYSTEM='"/system/lib64/libDLInterface_hidl.camera.samsung.so"'
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_VENDOR='"/vendor/lib64/libDLInterface.camera.samsung.so"' 
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATIONAIDL_SYSTEM='"/system/lib64/libDLInterface_aidl.camera.samsung.so"'
	#LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_VENDOR='"libDLInterface.camera.samsung.so"' 
else
	LOCAL_CFLAGS += -march=armv7-a -Da32_build
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_SYSTEM='"/system/lib/libDLInterface_hidl.camera.samsung.so"'
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_VENDOR='"/vendor/lib/libDLInterface.camera.samsung.so"'
endif

LOCAL_CFLAGS += -D__android__
LOCAL_CFLAGS += -D__IOD_ENGINE_VERSION__='"v2"'
LOCAL_CFLAGS += -D__PET_OD_VERSION__='"v2.16_MULTIFRAME"'
LOCAL_CFLAGS += -DUSE_NHWC_TENSOR
LOCAL_CFLAGS += -fno-var-tracking
LOCAL_CFLAGS += -DTEMPAVG
# LOCAL_CFLAGS += -DTESTING
# LOCAL_CFLAGS += -DDUMPBOX
# LOCAL_CFLAGS += -DDUMP_OUTPUT
# LOCAL_CFLAGS += -DDUMP_INPUT
LOCAL_CFLAGS += -DANDROID_ARM_NEON=TRUE
LOCAL_CFLAGS += -O3

#LOCAL_CLANG_TIDY := true
#LOCAL_CLANG_TIDY_FLAGS := -format-style=file
LOCAL_LDFLAGS := -fuse-ld=lld
LOCAL_LDFLAGS = "-Wl,-z,max-page-size=16384,-O3"
LOCAL_LDLIBS := -lz -llog -lc -pthread

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
# LOCAL_CFLAGS += -DLIBBUILDMODE='"hwasan"'
# LOCAL_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer
# LOCAL_LDFLAGS += -fsanitize=hwaddress
endif

LOCAL_SHARED_LIBRARIES += OPENCV_HQ
LOCAL_SHARED_LIBRARIES += STL_LIBC++

LOCAL_C_INCLUDES := $(OPENCV_INC_PATH) \
					$(JSON_CPP_INCLUDE) \
					$(DL_INTERFACE_INCLUDE) \
					$(EIGEN_CPP_INCLUDE) \
					$(EIGEN_CPP_INCLUDE)/Eigen \
					$(OBJECT_DETECTOR_INCLUDE) \
					$(ENGINE_CORE_INCLUDE) \
					$(ENGINE_SRC_INCLUDE) \
					$(ENGINE_TRACKER_INCLUDE) \
					$(STL_INCLUDE)

FILE_LIST := $(wildcard $(ENGINE_SRC_PATH)/*.cpp)
FILE_LIST += $(wildcard $(ENGINE_SRC_PATH)/**/*.cpp)
FILE_LIST += $(wildcard $(ENGINE_SRC_PATH)/**/**/*.cpp)
FILE_LIST += $(wildcard $(ENGINE_SRC_PATH)/**/**/**/*.cpp)
FILE_LIST += $(wildcard $(ENGINE_SRC_PATH)/**/**/**/**/*.cpp)
FILE_LIST += $(wildcard $(ENGINE_SRC_PATH)/**/**/**/**/*.cc)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
include $(BUILD_SHARED_LIBRARY)

#------------------------------------------------------
include $(CLEAR_VARS)
LOCAL_MODULE := PetDetectorJNI
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -DLIBBUILDMODE='"default"'

LOCAL_CFLAGS += -D__android__
LOCAL_CFLAGS += -D__IODJNI_VERSION__='"v1"'
LOCAL_CFLAGS += -fvisibility=hidden 

LOCAL_LDFLAGS := -fuse-ld=lld
LOCAL_LDFLAGS = "-Wl,-z,max-page-size=16384"
LOCAL_LDLIBS := -lz -llog -lc -pthread

LOCAL_SHARED_LIBRARIES += OPENCV_HQ
LOCAL_SHARED_LIBRARIES += PetDetector_v1.camera.samsung
LOCAL_SHARED_LIBRARIES += STL_LIBC++
ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
# LOCAL_CFLAGS += -DLIBBUILDMODE='"hwasan"'
# LOCAL_CFLAGS += -fsanitize=hwaddress -fno-omit-frame-pointer
# LOCAL_LDFLAGS += -fsanitize=hwaddress
endif
LOCAL_C_INCLUDES := $(OPENCV_INC_PATH) \
					$(JSON_CPP_INCLUDE) \
					$(STL_INCLUDE) \
					$(OBJECT_DETECTOR_INCLUDE) \
					$(ENGINE_CORE_INCLUDE)
					
LOCAL_SRC_FILES := $(LOCAL_PATH)/jni_wrapper_utils.cpp $(LOCAL_PATH)/ObjectDetectorJNI.cpp
include $(BUILD_SHARED_LIBRARY)

#-------------------------------------------------------
#hwasan


