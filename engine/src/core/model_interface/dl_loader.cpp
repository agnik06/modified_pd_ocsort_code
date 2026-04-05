#include "dl_loader.hpp"
#include <dlfcn.h>
#include "logger/logger.hpp"
#include <stdexcept>

#ifndef LOAD_DUMMY_DL_LOADER

void* get_dlinterface_lib_handle() {
  void* dlinterface_lib_handle=NULL;

  dlinterface_lib_handle = dlopen("libDLInterface.camera.samsung.so", RTLD_NOW | RTLD_GLOBAL);
  if (dlinterface_lib_handle != NULL) {
    LOGV("VZ Debug: Dynamic loading of libDLInterface successfull from LD DIR");
  }
  else{ 
    LOGV("VZ Debug: Dynamic loading of libDLInterface successfull from LD DIR failed");
  }

#ifdef DLINTERFACELIBLOCATION_SYSTEM
	if(dlinterface_lib_handle == NULL){
		dlinterface_lib_handle = dlopen(DLINTERFACELIBLOCATION_SYSTEM, RTLD_NOW | RTLD_GLOBAL);
    if (dlinterface_lib_handle != NULL) {
      LOGV("VZ Debug: Dynamic loading of libDLInterface successfull from %s",DLINTERFACELIBLOCATION_SYSTEM);
    }
		else{
      LOGV("VZ Debug: Dynamic loading of libDLInterface from %s failed",DLINTERFACELIBLOCATION_SYSTEM);
		}
	}
#endif

#ifdef DLINTERFACELIBLOCATIONAIDL_SYSTEM
	if(dlinterface_lib_handle == NULL){
		dlinterface_lib_handle = dlopen(DLINTERFACELIBLOCATIONAIDL_SYSTEM, RTLD_NOW|RTLD_GLOBAL);
    if (dlinterface_lib_handle != NULL) {
      LOGV("VZ Debug: Dynamic loading of libDLInterface_aidl.camera.samsung.so successful from %s",DLINTERFACELIBLOCATIONAIDL_SYSTEM);
    }
		else{
      LOGV("VZ Debug: Dynamic loading of libDLInterface_aidl.camera.samsung.so from %s failed",DLINTERFACELIBLOCATIONAIDL_SYSTEM);
		}
	}
#endif

#ifdef DLINTERFACELIBLOCATION_VENDOR
	if(dlinterface_lib_handle == NULL){
		dlinterface_lib_handle = dlopen(DLINTERFACELIBLOCATION_VENDOR, RTLD_NOW | RTLD_GLOBAL);
		if (dlinterface_lib_handle != NULL) {
			LOGV("VZ Debug: Dynamic loading of libDLInterface successfull from %s",DLINTERFACELIBLOCATION_VENDOR);
		}
  }
#endif

  try{
  if (dlinterface_lib_handle == NULL) {
    LOGV("VZ Debug: Dynamic loading of libDLInterface failed.\nERROR: %s", dlerror());
    throw std::runtime_error("Dynamic loading of libDLInterface failed.");
    }
  } catch(std::runtime_error& ex){
      LOGV("VZ Debug: Caught Std runtime Exception - %s", ex.what());
  } catch (std::exception& ex) {
      LOGV("VZ Debug: Caught Exception - %s", ex.what());
  }
  return dlinterface_lib_handle;
}


DLIWrapper* (*get_create_fptr(void* handle)) () {
	auto DLIWrapper_create_ptr = (DLIWrapper* (*)()) (dlsym(handle, "DLIWrapper_create"));
	if(DLIWrapper_create_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_create symbol not found");
	}
	return DLIWrapper_create_ptr;
}

bool (*get_open_fptr(void* handle)) (DLIWrapper*, const char*, const char*, char**, int, int, int) {
	auto DLIWrapper_openDLModel_ptr = (bool (*)(DLIWrapper*, const char*, const char*, char**, int, int, int)) (dlsym(handle, "DLIWrapper_openDLModel"));
	if(DLIWrapper_openDLModel_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_openDLModel symbol not found");
	}
	return 	 DLIWrapper_openDLModel_ptr;
}

bool (*get_open_fptr_direct_latency(void* handle)) (DLIWrapper*, const char*, char**, int, int, int, int) {
	auto DLIWrapper_openDLModel_ptr = (bool (*)(DLIWrapper*, const char*, char**, int, int, int,int)) (dlsym(handle, "DLIWrapper_openDLModel_direct_latency"));
	if(DLIWrapper_openDLModel_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_openDLModel_direct_latency symbol not found");
	}
	return 	 DLIWrapper_openDLModel_ptr;
}



bool (*get_open_fptr_snap_options(void* handle)) (DLIWrapper*, const char*, SNAPInterface ) {
	auto DLIWrapper_openDLModel_ptr = (bool (*)(DLIWrapper*, const char*, SNAPInterface)) (dlsym(handle, "DLIWrapper_openDLModel_snapOptions"));
	if(DLIWrapper_openDLModel_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_openDLModel_snapOptions symbol not found");
	}
	return DLIWrapper_openDLModel_ptr;
}

bool (*get_open_fptr_direct(void* handle)) (DLIWrapper*, const char*, char**, int, int, int) {
	auto DLIWrapper_openDLModel_ptr = (bool (*)(DLIWrapper*, const char*, char**, int, int, int)) (dlsym(handle, "DLIWrapper_openDLModel_direct"));
	if(DLIWrapper_openDLModel_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_openDLModel_direct symbol not found");
	}
	return 	 DLIWrapper_openDLModel_ptr;
}

bool (*get_inputDimens_fptr(void* handle)) (DLIWrapper*, int&, int&) {
	auto DLIWrapper_inputDimens_fptr = (bool (*)(DLIWrapper*, int&, int&)) (dlsym(handle, "DLIWrapper_getInputDimens"));
	if(DLIWrapper_inputDimens_fptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_inputDimens_fptr symbol not found");
	}
	return 	 DLIWrapper_inputDimens_fptr;
}

bool (*get_inputChannels_fptr(void* handle)) (DLIWrapper*, int&) {
	auto DLIWrapper_inputChannels_fptr = (bool (*)(DLIWrapper*, int&)) (dlsym(handle, "DLIWrapper_getInputChannels"));
	if(DLIWrapper_inputChannels_fptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_inputChannels_fptr symbol not found");
	}
	return 	 DLIWrapper_inputChannels_fptr;
}

bool (*get_execute_fptr(void* handle)) (DLIWrapper*, float*, int, int, int) {
	auto DLIWrapper_execute_ptr = (bool (*)(DLIWrapper*, float*, int, int, int)) (dlsym(handle, "DLIWrapper_execute"));
	if(DLIWrapper_execute_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_execute symbol not found");
	}
	return 	DLIWrapper_execute_ptr;
}

bool (*get_execute_multi_inp_fptr(void* handle)) (DLIWrapper*, std::vector<float*>, std::vector<std::vector<int>>) {
	auto DLIWrapper_execute_ptr = (bool (*)(DLIWrapper*, std::vector<float*>, std::vector<std::vector<int>>)) (dlsym(handle, "DLIWrapper_execute_multiple"));
	if(DLIWrapper_execute_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_execute_multiple for multi_inp symbol not found");
	}
	return 	DLIWrapper_execute_ptr;
}

float* (*get_output_fptr(void* handle)) (DLIWrapper*, const char*, int&, int&, int&, int&) {
	auto DLIWrapper_getLayerOutput_ptr = (float* (*)(DLIWrapper*, const char*, int&, int&, int&, int&)) (dlsym(handle, "DLIWrapper_getLayerOutput"));
	if(DLIWrapper_getLayerOutput_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_getLayerOutput symbol not found");
	}

	return 	DLIWrapper_getLayerOutput_ptr;
}

float* (*get_output_shape_fptr(void* handle)) (DLIWrapper*, const char*, std::vector<int>&) {
	auto DLIWrapper_getLayerOutputShape_ptr = (float* (*)(DLIWrapper*, const char*, std::vector<int>&)) (dlsym(handle, "DLIWrapper_getLayerOutputAndShape"));
	if(DLIWrapper_getLayerOutputShape_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_getLayerOutputAndShape symbol not found");
	}

	return 	DLIWrapper_getLayerOutputShape_ptr;
}

std::vector<float*> (*get_multi_layer_output_fptr(void* handle)) (DLIWrapper*, const std::vector<char*>, std::vector<std::vector<int>>&) {
	auto DLIWrapper_getMultiLayerOutput_ptr = (std::vector<float*> (*)(DLIWrapper*, const std::vector<char*>, std::vector<std::vector<int>>&)) (dlsym(handle, "DLIWrapper_getMultiLayerOutput"));
	if(DLIWrapper_getMultiLayerOutput_ptr == NULL) {
		// LOGV("VZ Debug: DLIWrapper_getMultiLayerOutput symbol not found");
		LG<<"VZ Debug: DLIWrapper_getMultiLayerOutput symbol not found";
	}

	return 	DLIWrapper_getMultiLayerOutput_ptr;
}

float* (*get_output_and_format_fptr(void* handle)) (DLIWrapper*, const char*, int&, int&, int&, int&, int&) {
	auto DLIWrapper_getLayerOutputAndFormat_ptr = (float* (*)(DLIWrapper*, const char*, int&, int&, int&, int&, int&)) (dlsym(handle, "DLIWrapper_getLayerOutputAndFormat"));
	if(DLIWrapper_getLayerOutputAndFormat_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_getLayerOutputAndFormat symbol not found");
	}

	return 	DLIWrapper_getLayerOutputAndFormat_ptr;
}

void (*get_destroy_fptr(void* handle)) (DLIWrapper*) {
	auto DLIWrapper_destroy_ptr = (void (*)(DLIWrapper*)) (dlsym(handle, "DLIWrapper_destroy"));
	if(DLIWrapper_destroy_ptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_destroy symbol not found");
	}
	return 	DLIWrapper_destroy_ptr;
}

bool (*get_setChannelMeans_fptr(void* handle)) (DLIWrapper* , float* , int ){
	auto DLIWrapper_setChannelMeans_fptr = (bool (*)(DLIWrapper* , float* , int)) (dlsym(handle, "DLIWrapper_setChannelMeans"));
	if(DLIWrapper_setChannelMeans_fptr == NULL) {
		LOGV("VZ Debug: DLIWrapper_setChannelMeans_fptr symbol not found");
	}
	return DLIWrapper_setChannelMeans_fptr;
}

#endif
