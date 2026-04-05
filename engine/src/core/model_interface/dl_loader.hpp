#ifndef DL_LOADER_HPP
#define DL_LOADER_HPP
#include "DLInterface_C_Wrapper.hpp"

//#define LOAD_DUMMY_DL_LOADER

void* get_dlinterface_lib_handle();
DLIWrapper* (*get_create_fptr(void* handle)) ();
bool (*get_open_fptr(void* handle)) (DLIWrapper*, const char*, const char*, char**, int, int, int);

bool (*get_open_fptr_direct(void* handle)) (DLIWrapper*, const char*, char**, int, int, int);
bool (*get_open_fptr_direct_latency(void* handle)) (DLIWrapper*, const char*, char**, int, int, int,int);
bool (*get_open_fptr_snap_options(void* handle)) (DLIWrapper*, const char*, SNAPInterface );
bool (*get_inputDimens_fptr(void* handle)) (DLIWrapper*, int&, int&);
bool (*get_inputChannels_fptr(void* handle)) (DLIWrapper*, int&);
bool (*get_execute_fptr(void* handle)) (DLIWrapper*, float*, int, int, int);
bool (*get_execute_multi_inp_fptr(void* handle)) (DLIWrapper*, std::vector<float*>, std::vector<std::vector<int>>);
float* (*get_output_fptr(void* handle)) (DLIWrapper*, const char*, int&, int&, int&, int&);
float* (*get_output_shape_fptr(void* handle)) (DLIWrapper*, const char*, std::vector<int>&);

std::vector<float*> (*get_multi_layer_output_fptr(void* handle)) (DLIWrapper*, const std::vector<char*>, std::vector<std::vector<int>>&);
float* (*get_output_and_format_fptr(void* handle)) (DLIWrapper*, const char*, int&, int&, int&, int&, int&);
void (*get_destroy_fptr(void* handle)) (DLIWrapper*);

bool (*get_setChannelMeans_fptr(void* handle)) (DLIWrapper* handle, float* channel_means, int num_channels);
#endif
