#ifndef DLINTERFACE_C_WRAPPER_HPP
#define DLINTERFACE_C_WRAPPER_HPP
#include <stdint.h>
#include <vector>
#include <string>

//#include "snap_interface.h"

#ifdef __cplusplus
extern "C" {
#endif
struct DLIWrapper;
typedef DLIWrapper DLIWrapper;

struct VersionInfoParams {
	typedef enum {
		MODEL_FILE = 0,
		CACHE_FILE = 1,
	}FileType;
	typedef enum {
		PATH = 0,
		FD = 1,
		BUFFER = 2,
	}BufferType;

	FileType fileType;
	BufferType bufferType;
	std::string path;
	int32_t fd;
	uint8_t *buffer;
	int computeUnit;
	int modelType;
};

struct VersionInfoWrapper {
  int compUnit;
  int modelType;
  std::string version;
};

typedef std::vector<VersionInfoWrapper> VersionInfoWrapperList;

#ifndef SNAP_INTERFACE
#define SNAP_INTERFACE
typedef struct _SNAPInterface_{
	typedef enum {
		SNAP_CPU = 0,
		SNAP_GPU = 1,
		SNAP_DSP = 2,
		SNAP_NPU = 3,
		SNAP_UNIT_MAXENUM = 4,
	}SNAPCompUnit;

	typedef enum {
		SNAP_CAFFE = 0,
		SNAP_SNPE = 1,
		SNAP_EDEN = 2,
		SNAP_TENSORFLOW = 3,
		SNAP_TENSORFLOWLITE = 4,
		SNAP_OFI = 5,
		SNAP_SNF = 6,
    SNAP_HVXNN = 7,
    SNAP_ONNX = 8,
    SNAP_SNAPLITE = 9,
    SNAP_TVM = 10,
    SNAP_MNP = 11,
		SNAP_MODEL_MAXENUM = 12,
	}SNAPModelType;

	typedef enum {
		SNAP_FLOAT32,
		SNAP_FLOAT16,
		SNAP_QASYMM16,
		SNAP_QASYMM8,
    SNAP_BFLOAT16,
		SNAP_DATATYPE_MAXENUM,
	}SNAPExecType;

	typedef enum {
		SNAP_HIGH_RESOLUTION,
    SNAP_FAST_INFERENCE,
    SNAP_BALANCED_INFERENCE,
		SNAP_OPTIONS_MAXENUM,
	}SNAPAddOptions;

	typedef enum {
		SNAP_DEBUG,
		SNAP_RELEASE,
		SNAP_MODE_MAXENUM,
	}SNAPMode;

	typedef enum {
		SNAP_SUSTAINED_PERFORMANCE,
		SNAP_HIGH_PERFORMANCE,
		SNAP_LOW_PERFORMANCE,
		SNAP_DVFS_MAXENUM,
	}SNAPDvfs;
 
  typedef enum {
    SNAP_DEFAULT,
    SNAP_CPU_XTENSIVE,
    SNAP_CPU_COMPUTE,
    SNAP_GPU_COMPUTE_CACHE,
    SNAP_GPU_COMPUTE_XTENSIVE_CACHE,
    SNAP_GPU_CL_CACHE,
    SNAP_SNAPLITE_CATALYST_MAXENUM,
  }SNAPSnapliteCatalyst;

	SNAPCompUnit compUnit = SNAP_UNIT_MAXENUM;
	SNAPExecType execType = SNAP_DATATYPE_MAXENUM;
	SNAPModelType mType = SNAP_MODEL_MAXENUM;
	std::vector<std::string> inputNames = {};
	std::vector<std::string> outputNames = {};        
	std::vector<float> scale = {};
	std::vector<float> mean = {};
	std::vector<float> stddev = {};
    std::string model_file = "";
	std::string weights_file = "";
	int cpuThreadCount = 0;
	bool allowReshape = false;
	std::vector<uint8_t> model_buffer = {};
	bool enable_secure = false;
	char *gpuCacheBuffer = 0;
	SNAPAddOptions flag = SNAP_OPTIONS_MAXENUM;
	std::string model_name = "";
	std::vector<std::vector<int32_t>> input_shape = {};
	SNAPMode mode = SNAP_MODE_MAXENUM;
  SNAPSnapliteCatalyst snaplite_catalyst = SNAP_DEFAULT;
	SNAPDvfs dvfs_perf = SNAP_DVFS_MAXENUM;
	size_t cacheSize = 0;
	uint8_t *model_buffer_ptr = 0; /* This is for optimized implementation. */
	size_t model_buffer_len = 0;
  int model_buffer_id = -1;	
}SNAPInterface;
#endif

//typedef SNAPInterfaceWrapper SNAPInterface;


DLIWrapper* DLIWrapper_create();


DLIWrapper* DLIWrapper_create();
void DLIWrapper_destroy(DLIWrapper* handle);
bool DLIWrapper_openDLModel(DLIWrapper* handle, const char* filePath,
                            const char* moduleName, char** layer_names,
                            int size, int compute_unit_preference, int execution_data_type_preference);
bool DLIWrapper_openDLModel_direct(DLIWrapper* handle, const char* filePath, char** layer_names,
                            int size, int compute_unit_preference, int execution_data_type_preference);
bool DLIWrapper_openDLModel_direct_latency(
		DLIWrapper* handle, const char* filePath, char** layer_names, int size,
		int compute_unit_preference, int execution_data_type_preference,
		int latency);
bool DLIWrapper_openDLModelfromBuffer(DLIWrapper* handle, int model_buffer_fd, int gpu_cache_buffer_fd,char** layer_names,
                            int size, const char* model_name, int compute_unit_preference, int execution_data_type_preference, int 			model_type_information);
bool DLIWrapper_openDLModelfromBuffer_latency(
		DLIWrapper* handle, int model_buffer_fd, int gpu_cache_buffer_fd,
		char** layer_names, int size, const char* model_name,
		int compute_unit_preference, int execution_data_type_preference,
		int model_type_information, int latency);
bool DLIWrapper_openDLModel_snapOptions(
		DLIWrapper* handle, const char* filePath, SNAPInterface options);
bool DLIWrapper_getInputDimens(DLIWrapper* handle, int& width, int& height);
bool DLIWrapper_getInputChannels(DLIWrapper* handle, int& channels); 
bool DLIWrapper_setChannelMeans(DLIWrapper* handle, float* channel_means, int num_channels);
bool DLIWrapper_execute(DLIWrapper* handle, float* imageData, int width,
                        int height, int imageFormat);
bool DLIWrapper_execute_multiple(DLIWrapper* handle, std::vector<float*> &input_buffers, 
			std::vector<std::vector<int>> &shapes) ;
float* DLIWrapper_getLayerOutput(DLIWrapper* handle, const char* layerName,
                                 int& b, int& c, int& h, int& w);
float* DLIWrapper_getLayerOutputAndShape(DLIWrapper* handle, const char* layerName, std::vector<int>& output_shape);
//dataFormat: NCHW: 0, NHWC: 1
float* DLIWrapper_getLayerOutputAndFormat(DLIWrapper* handle, const char* layerName,
                                 int& b, int& c, int& h, int& w, int& dataFormat);

bool DLIWrapper_getLayerInputInfo(DLIWrapper* handle, const char* layerName,
                                  int& c, int& h, int& w);

std::vector<float*> DLIWrapper_getMultiLayerOutput(const std::vector<char*> layerNames, std::vector<std::vector<int>>& output_shape);
const char* DLIWrapper_getSnapBufferVersion(DLIWrapper* handle, VersionInfoParams params);
const char* DLIWrapper_getSnapVersion(DLIWrapper* handle, int computeUnit, int snapModelType);
VersionInfoWrapperList* DLIWrapper_getAllSupportedSnapVersions(DLIWrapper* handle);


#ifdef __cplusplus
}
#endif
#endif //DLINTERFACE_C_WRAPPER_HPP
