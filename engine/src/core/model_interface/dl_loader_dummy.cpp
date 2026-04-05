#include "dl_loader.hpp"
#include <dlfcn.h>
#include "logger/logger.hpp"
#include <stdexcept>

#ifdef LOAD_DUMMY_DL_LOADER

void* get_dlinterface_lib_handle() {
    return NULL;
}

DLIWrapper* get_create_fptr_fn() {
	return NULL;
}

DLIWrapper* (*get_create_fptr(void* handle)) () {
	return get_create_fptr_fn;
}

bool get_open_fptr_fn(DLIWrapper*, const char*, const char*, char**, int, int, int){
	return true;
}


bool (*get_open_fptr(void* handle)) (DLIWrapper*, const char*, const char*, char**, int, int, int) {
	return get_open_fptr_fn;
}


bool get_open_fptr_direct_latency_fn(DLIWrapper*, const char*, char**, int, int, int, int){
    return true;
}

bool get_open_fptr_direct_fn(DLIWrapper*, const char*, char**, int, int, int){
    return true;
}


bool (*get_open_fptr_direct_latency(void* handle)) (DLIWrapper*, const char*, char**, int, int, int, int){
	return get_open_fptr_direct_latency_fn;
}

bool (*get_open_fptr_direct(void* handle)) (DLIWrapper*, const char*, char**, int, int, int) {
	return get_open_fptr_direct_fn;
}

bool get_inputDimens_fptr_fn(DLIWrapper*, int& w, int& h) {
	w = 300;
	h = 300;
	return true;
}
bool (*get_inputDimens_fptr(void* handle)) (DLIWrapper*, int&, int&) {
	return get_inputDimens_fptr_fn;
}

bool get_execute_fptr_fn(DLIWrapper*, float*, int, int, int){
    return true;
}

bool (*get_execute_fptr(void* handle)) (DLIWrapper*, float*, int, int, int) {
	return 	get_execute_fptr_fn;
}

float* get_output_fptr_fn (DLIWrapper*, const char* layer_name, int& b, int& c, int& h, int& w) {
	if(std::string(layer_name) == "prob"){
		b = 1;
		c = 1122;
		h = 1;
		w = 1;
		static float output[1122];
		for(int i=0;i<1122;i++){
			output[i]=0;
		}
		output[708]=1;
		return output;
	}
	else if(std::string(layer_name) == "detection_out"){
		b = 1;
		c = 1;
		h = 2;
		w = 7;
		static float output[] = {0, 1 ,0.97786, -0.00246283 ,0.214037, 0.545131, 0.983198};
		return output;
	}
	return NULL;
}

bool get_setChannelMeans_fptr_fn(DLIWrapper*, float* channelmeans,int num_channels){
	return true;
}


bool get_inputChannels_fptr_fn(DLIWrapper*, int& num_channels){
	num_channels = 3;
	return true;
}


bool (*get_setChannelMeans_fptr(void* handle)) (DLIWrapper* , float* , int ){
	return get_setChannelMeans_fptr_fn;
}


bool (*get_inputChannels_fptr(void* handle)) (DLIWrapper*, int&){
	return get_inputChannels_fptr_fn;
}


float* (*get_output_fptr(void* handle)) (DLIWrapper*, const char*, int&, int&, int&, int&) {
	return 	get_output_fptr_fn;
}

void get_destroy_fptr_fn(DLIWrapper*) {
}

void (*get_destroy_fptr(void* handle)) (DLIWrapper*) {
	return 	get_destroy_fptr_fn;
}

void release_dlinterface_lib_handle(void* handle){
	
}

#endif