LOCAL_PATH := $(call my-dir)
ENGINE_BASE_PATH 				:= $(LOCAL_PATH)/../../../../../engine
ENGINE_SRC_PATH 				:= $(ENGINE_BASE_PATH)/src
ENGINE_THIRD_PARTY_LIBS_PATH 	:= $(ENGINE_BASE_PATH)/third_party
THIRD_PARTY_LIBS_PATH 			:= $(LOCAL_PATH)/../../../../light_object_detector/third_party/

#INCLUDE PATHS
OPENCV_INC_PATH 		:= $(THIRD_PARTY_LIBS_PATH)/pcv340/include
STL_INCLUDE 			:= $(THIRD_PARTY_LIBS_PATH)/stl_libc++/include/libcxx_inc
JSON_CPP_INCLUDE 		:= $(ENGINE_THIRD_PARTY_LIBS_PATH)/json
DL_INTERFACE_INCLUDE 	:= $(ENGINE_THIRD_PARTY_LIBS_PATH)/DLInterface/include/
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
LOCAL_MODULE := LightObjectDetectorTester
LOCAL_ARM_MODE := arm

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	LOCAL_CFLAGS += -march=armv8-a
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_SYSTEM='"/system/lib64/libDLInterface_hidl.camera.samsung.so"'
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_VENDOR='"/vendor/lib64/libDLInterface.camera.samsung.so"' 
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATIONAIDL_SYSTEM='"/system/lib64/libDLInterface_aidl.camera.samsung.so"'
	#LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_VENDOR='"libDLInterface.camera.samsung.so"' 
else
	LOCAL_CFLAGS += -mfpu=neon -march=armv7-a -DSS_NEON_ASSEMBLY -Da32_build
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_SYSTEM='"/system/lib/libDLInterface_hidl.camera.samsung.so"'
	LOCAL_CFLAGS += -DDLINTERFACELIBLOCATION_VENDOR='"/vendor/lib/libDLInterface.camera.samsung.so"'
endif

LOCAL_CFLAGS += -D__android__
LOCAL_CFLAGS += -D__terminal_logging__
LOCAL_CFLAGS += -D__IOD_ENGINE_VERSION__='"v1"'
LOCAL_CFLAGS += -D__LIGHTGENERIC_OD_VERSION__='"v1.0.0"'
LOCAL_CFLAGS += -DUSE_NHWC_TENSOR
LOCAL_CFLAGS += -fno-var-tracking
LOCAL_CFLAGS += -fvisibility=hidden 
# LOCAL_CFLAGS += -DTESTING

#LOCAL_CLANG_TIDY := true
#LOCAL_CLANG_TIDY_FLAGS := -format-style=file
LOCAL_LDFLAGS := -fuse-ld=gold
LOCAL_LDLIBS := -lz -llog -lc -pthread

LOCAL_SHARED_LIBRARIES += OPENCV_HQ
LOCAL_SHARED_LIBRARIES += STL_LIBC++

LOCAL_C_INCLUDES := $(OPENCV_INC_PATH) \
					$(JSON_CPP_INCLUDE) \
					$(DL_INTERFACE_INCLUDE) \
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
LOCAL_SRC_FILES += $(LOCAL_PATH)/test.cpp

include $(BUILD_EXECUTABLE)
#------------------------------------------------------
