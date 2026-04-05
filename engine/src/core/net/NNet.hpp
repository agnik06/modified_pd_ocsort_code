#ifndef NNET_HPP
#define NNET_HPP
#include <vector>
#include <opencv2/opencv.hpp>
#include <string>

#include "DLInterface_C_Wrapper.hpp"
#include "logger/logger.hpp"
#include "model_interface/dl_loader.hpp"
#include "model_config.hpp"
#include <dlfcn.h>
#include <thread>
#include <mutex>
#include <functional>

typedef std::function<void(bool, std::string)> NNetInitCallback;

class NNet{
    public:
        bool init_net(const std::string& file_path,
                    const std::string& moduleName,
                    std::vector<std::string> layer_names, 
                    std::vector<float> channel_means, 
                    int compute_unit_preference, 
                    int execution_data_type_preference, 
                    int latency,
                    std::vector<std::string> input_layer_names,
                    int resolution,
                    NNetInitCallback cb);
       
        NNet(const std::string& file_path, const std::string& moduleName, std::vector<std::string> layer_names, std::vector<float> channel_means, int compute_unit_preference, int execution_data_type_preference, int latency, std::vector<std::string> input_layer_names,int resoltion, NNetInitCallback cb);
        void get_target_dimen(int &t_height,int& t_width);
        bool forward_pass(cv::Mat image);
		float* get_layer_output(const char* layerName, int &b, int &c, int &h, int &w);
        float* get_layer_output_shape(const char* layerName, std::vector<int>& output_shape);
        float* get_layer_output_internal(const char* layerName, std::vector<int>& output_shape);
        float* get_layer_output_with_format(const char* layerName, int &b, int &c, int &h, int &w, int &data_format);
        std::vector<float*> get_layer_multi_output_internal(const std::vector<char*> layerName, std::vector<std::vector<int>>& output_shape);

        void set_snap_options(const std::string& file_path, 
					const std::string& module_name,
					std::vector<std::string> layer_names, 
                    std::vector<float> channel_means, 
                    int compute_unit_preference, 
                    int execution_data_type_preference, 
                    int latency,
                    std::vector<std::string> input_layer_names);
        bool get_open_status();
        void set_model_Version(model::Version version);
        ~NNet();

    protected:
        DLIWrapper* dli_handle;
        std::string module_name;
        void* dlinterface_lib_handle;
        SNAPInterface snap_options;
        int target_width, target_height, target_channels, original_width, original_height;
        bool open_status = false;
        bool execute_status;
        std::thread init_thread;
        std::mutex init_mutex;
        std::mutex deinit_mutex;
        int compute_unit;
        model::Version version;
        std::vector<float> channel_means;
        // std::vector<float> six_channel_means;
        std::vector<float> channel_scales;
        // std::vector<float> six_channel_scales;
        
        // cv::Mat prevFrame;
        int imgcnt;
        bool stop_fuse = false;
        int burn_time = 10000;
        std::thread time_fuse_thread;
        void init_time_fuse(const std::string& moduleName, int burn_time);//duration - in ms
        int get_init_burn_time(int compute_unit);

        bool set_channel_means(std::vector<float> means);

    public:
        class Builder{
        public:
            Builder();
            Builder& set_model_path(std::string model_path);
            Builder& set_module_name(std::string module_name);
            Builder& set_output_layer_names(std::vector<std::string> output_layer_names);
            Builder& set_compute_unit(int compute_unit);
            Builder& set_execution_data_type(int execution_data_type);
            Builder& set_channel_means(std::vector<float> means);
            Builder& set_latency(int _latency);
            Builder& set_input_layer_names(std::vector<std::string> input_layer_names);
            Builder& set_resolution(int _resolution);
            Builder& set_init_callback_fn(NNetInitCallback callback_fn);
            Builder& validate();
            std::shared_ptr<NNet> build();
            
        public:
            std::string model_path = "";
            std::string module_name = "";
            std::vector<std::string> output_layer_names;
            int compute_unit = -1;
            int execution_data_type = -1;
            int latency = 0;
            std::vector<float> channel_means;
            std::vector<std::string> input_layer_names;
            int resolution = 0;
            NNetInitCallback init_callback_fn = NULL;
        };
        
        NNet(NNet::Builder& builder);

        const int size_128 = 128 * 128 * 56;
        const int size_64 = 64 * 64 * 112;
        const int size_32 = 32 * 32 * 224;
        const int size_16 = 16 * 16 * 448;

        float *inter_state_128;
        float *inter_state_64;
        float *inter_state_32;
        float *inter_state_16;
        std::vector<float*> multi_input_buffers;
        std::vector<std::vector<int>> multi_input_shapes;
        // float *dummy_output_for_debug;
        
        std::vector<std::string> output_layer_names_str;
        char output_layer_names[5][20];
        std::vector<char*> layer_names_cstr;
        
};

#endif /* NNET_HPP */
