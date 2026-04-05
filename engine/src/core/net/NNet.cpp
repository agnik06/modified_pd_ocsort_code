#include "NNet.hpp"
#include "model_interface/image_preprocessor.hpp"
#include <stdexcept>
#include "logger/timer.hpp"
#include "logger/error.hpp"
#include "utils/sys_utils.hpp"
#include "utils/image_utils.hpp"

#ifdef LOAD_DUMMY_DL_LOADER
#include <fstream>
#endif

bool NNet::init_net(const std::string& file_path, 
				const std::string& module_name, 
				std::vector<std::string> layer_names, 
				std::vector<float> channel_means, 
				int compute_unit_preference, 
				int execution_data_type_preference, 
				int latency,
				std::vector<std::string> input_layer_names,
				int resoltion,
				NNetInitCallback cb){
	int start_time = CURRENT_TIME;
	std::lock_guard<std::mutex> guard(init_mutex);
	LOGV("VZ Debug: NNet Constructor called for %s, %s", file_path.c_str(), module_name.c_str());
	open_status = false;
	execute_status = false;
	this->module_name = module_name;
	this->compute_unit = compute_unit_preference;
	target_channels = 3; //default, gets changed in 'set_channel_means()'

	inter_state_128 = new float[size_128]{0.0f};
	inter_state_64 = new float[size_64]{0.0f};
	inter_state_32 = new float[size_32]{0.0f};
	inter_state_16 = new float[size_16]{0.0f};
	/*dummy_output_for_debug = new float[1*21824*39*1]{0.0f};
	std::string raw_input_path = "/sdcard/Download/video_0.raw";
	LG<<"VZ Debug: Overriding and Reading input path = " << raw_input_path;
	std::ifstream in(raw_input_path,std::ios::in| std::ios::binary);
	in.read((char*)dummy_output_for_debug, sizeof(float)*1*21824*39*1);*/

	//layer_names type conversion from vector<std::string> to char**
    
    for(int i = 0; i<layer_names.size(); i++) {
		std::strcpy(output_layer_names[i], layer_names[i].c_str());
    	layer_names_cstr.push_back(output_layer_names[i]);
    }
    char** layer_names_ptr = NULL;
    if(!layer_names_cstr.empty()) {
    	layer_names_ptr = &layer_names_cstr[0];
    }

	std::vector<char*> input_layer_names_cstr;
    for(auto layer_name: input_layer_names) {
    	input_layer_names_cstr.push_back(const_cast<char*> (layer_name.c_str()));
    }

    char** input_layer_names_ptr = NULL;
    if(!input_layer_names_cstr.empty()) {
    	input_layer_names_ptr = &input_layer_names_cstr[0];
    }
	//function pointers of dl-interface
    dlinterface_lib_handle = get_dlinterface_lib_handle();
	if(dlinterface_lib_handle == NULL){
		open_status = false;
		return false;
	}
    auto DLIWrapper_create_ptr = get_create_fptr(dlinterface_lib_handle);
    auto DLIWrapper_openDLModel_ptr = get_open_fptr(dlinterface_lib_handle);
    auto DLIWrapper_openDLModel_ptr_direct_latency = get_open_fptr_direct_latency(dlinterface_lib_handle);
	auto DLIWrapper_openDLModel_ptr_snapOptions = get_open_fptr_snap_options(dlinterface_lib_handle);
	auto DLIWrapper_openDLModel_ptr_direct = get_open_fptr_direct(dlinterface_lib_handle);
    auto DLIWrapper_get_inputDimens_fptr = get_inputDimens_fptr(dlinterface_lib_handle);
    auto DLIWrapper_get_inputChannels_fptr = get_inputChannels_fptr(dlinterface_lib_handle);
	if(DLIWrapper_create_ptr)
	dli_handle = DLIWrapper_create_ptr();
	else
	dli_handle = NULL;
	bool status = (dli_handle != NULL);
	imgcnt = 0;
	#ifdef LOAD_DUMMY_DL_LOADER
	status= true;
	#endif

	multi_input_buffers.resize(5);
	multi_input_shapes.resize(5);

	std::string model_extention = file_path.substr(file_path.rfind("."));
	if(this->compute_unit == 3 && model_extention!=".dla")
	{
		multi_input_shapes[0] = std::vector<int>{512,512,3};
		multi_input_shapes[1] = std::vector<int>{128,128,56};
		multi_input_shapes[2] = std::vector<int>{64,64,112};
		multi_input_shapes[3] = std::vector<int>{32,32,224};
		multi_input_shapes[4] = std::vector<int>{16,16,448};
	}
	else
	{
		multi_input_shapes[0] = std::vector<int>{1,512,512,3};
		multi_input_shapes[1] = std::vector<int>{1,128,128,56};
		multi_input_shapes[2] = std::vector<int>{1,64,64,112};
		multi_input_shapes[3] = std::vector<int>{1,32,32,224};
		multi_input_shapes[4] = std::vector<int>{1,16,16,448};
	}
	

    if(status) {
		//set_channel_means(channel_means);
		// six_channel_means = {128.0, 128.0, 128.0, 128.0, 128.0, 128.0};
		// six_channel_scales = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
		this->channel_scales = std::vector<float>(channel_means.size(), 1.0);
		this->channel_means = channel_means;
		try {
#if BUILD_x86			
			std::string base_path = get_final_models_base_path(file_path, module_name);
			status = DLIWrapper_openDLModel_ptr(dli_handle, base_path.c_str(), module_name.c_str(), layer_names_ptr, layer_names_cstr.size(), compute_unit_preference, execution_data_type_preference);
#else
			bool has_full_path = (file_path.rfind(".") != std::string::npos);
			std::string model_extention = file_path.substr(file_path.rfind("."));
			if(has_full_path){
				
				if(DLIWrapper_openDLModel_ptr_snapOptions){
					
					set_snap_options(file_path, 
									module_name, 
									layer_names, 
									channel_means, 
									compute_unit_preference, 
									execution_data_type_preference, 
									latency,
									input_layer_names);
					status = DLIWrapper_openDLModel_ptr_snapOptions(dli_handle, file_path.c_str(),snap_options);
					
					if(!status){
						LG<<"VZ DEBUG: failed to open mdels using snap_options_api";
					}
				}
				else if(DLIWrapper_openDLModel_ptr_direct_latency){
					LG<<"failed to get DLIWrapper_openDLModel_ptr_snapOptions";
	    			status = DLIWrapper_openDLModel_ptr_direct_latency(dli_handle, file_path.c_str(), layer_names_ptr, layer_names_cstr.size(), compute_unit_preference, execution_data_type_preference,latency);
				}
				else{
					LG<<"failed to pass latency to snap in dlinterface";
					set_channel_means(channel_means);
					status = DLIWrapper_openDLModel_ptr_direct(dli_handle, file_path.c_str(), layer_names_ptr, layer_names_cstr.size(), compute_unit_preference, execution_data_type_preference);
				}
			}
			else{
				std::string base_path = get_final_models_base_path(file_path, module_name);
	    		status = DLIWrapper_openDLModel_ptr(dli_handle, base_path.c_str(), module_name.c_str(), layer_names_ptr, layer_names_cstr.size(), compute_unit_preference, execution_data_type_preference);
			}
#endif
		}
		catch(std::exception& e) {
			status = false;
			LOGV("DLInterface: SNAP session returned NULL ! Exception thrown in open for module %s | %s", module_name.c_str(), e.what());
		}
	}
	if(status) {
		// status = DLIWrapper_get_inputDimens_fptr(dli_handle, target_width, target_height);
		if(DLIWrapper_get_inputDimens_fptr!= NULL){
			status = DLIWrapper_get_inputDimens_fptr(dli_handle, target_width, target_height);
		}
		else{
			status = false;
		}
			//DLIWrapper_get_inputChannels_fptr(dli_handle, target_channels);			
	}
	LOGV("VZ Debug: NNet Contructor %s for module - %s", status?"completed":"failed", module_name.c_str());

	int end_time = CURRENT_TIME;
	// LOGV("VZ Debug: engine-compute: Open time taken: %d ms for module - %s", (end_time - start_time), module_name.c_str());
	LG<<"VZ Debug: engine-compute: Open time taken: "<< (end_time - start_time) <<" ms for module - "<< module_name.c_str();
	LOGV("VZ Debug: open_status=[%d], status[%d]", open_status, status);

	open_status = status;
	if(cb!=NULL){
		cb(open_status, module_name);
	}	
	this->stop_fuse = true;
	return open_status;
}

NNet::NNet(const std::string& file_path, const std::string& module_name, std::vector<std::string> layer_names, std::vector<float> channel_means, int compute_unit_preference, int execution_data_type_preference,int latency, std::vector<std::string> input_layer_names,int resoltion, NNetInitCallback cb){
	target_height=480;
	target_width=640;
	if(cb == NULL){
		init_net(file_path, module_name, layer_names, channel_means, compute_unit_preference, execution_data_type_preference,latency, input_layer_names, resoltion, NULL);
	}
	else{
		init_thread = std::move(std::thread(&NNet::init_net, this, file_path, module_name, layer_names, channel_means, compute_unit_preference, execution_data_type_preference,latency, input_layer_names, resoltion, cb));
		time_fuse_thread = std::move(std::thread(&NNet::init_time_fuse, this, module_name, this->get_init_burn_time(compute_unit_preference)));
	}
}

bool NNet::set_channel_means(std::vector<float> means){
	if(means.empty()){
		return false;
	}
	// target_channels = means.size();
	this -> channel_means = means;
	this -> channel_scales = std::vector<float>(means.size(), 1.0);
	LG<<"VZ Debug: NNet::channel_scales 0= "<<channel_scales[0];
	LG<<"VZ Debug: NNet::channel_scales 1= "<<channel_scales[1];
	LG<<"VZ Debug: NNet::channel_scales 2= "<<channel_scales[2];

    auto DLIWrapper_setChannelMeans_ptr = get_setChannelMeans_fptr(dlinterface_lib_handle);
	float* means_array = &means[0];
	return DLIWrapper_setChannelMeans_ptr(dli_handle, means_array, means.size());
}

void NNet::init_time_fuse(const std::string& module_name, int burn_time){
	int total_sleep_time = 0;
	int min_sleep_time = std::min(100, burn_time);
	LG<<"VZ Debug: PD init_time_fuse_start for "<<module_name.c_str();
	LG<<"VZ Debug: PD maximum_allowed_burn_time_for "<<module_name.c_str()<<" ="<<burn_time<<"ms";
	while(total_sleep_time < burn_time){
		int sleep_time = std::min(100, burn_time - total_sleep_time);//ms
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
		total_sleep_time += sleep_time;
		LG<<"VZ Debug: init_time_fuse for "<<module_name.c_str()<<", time taken="<<total_sleep_time;
		if(this->stop_fuse){
			LG<<"VZ Debug: init_time_fuse_stop for"<<module_name.c_str()<<", time taken="<<total_sleep_time;
			return;
		}
	}
	return;
	// throw vizpet::ModelInitError("Model initialization for "+ module_name +" is taking more than "+std::to_string(burn_time)+ " ms. Expected shorter initilization time.");
}


int NNet::get_init_burn_time(int compute_unit_preference){
	snap::ComputeUnit compute_unit = static_cast<snap::ComputeUnit>(compute_unit_preference);
	if(compute_unit == snap::ComputeUnit::GPU){
		return 60000;
	}
	return this->burn_time;
}

void NNet::get_target_dimen(int &t_width,int& t_height){
	t_width=target_width;
	t_height=target_height;
	return;
}

void NNet::set_snap_options(const std::string& file_path, 
					const std::string& module_name,
					std::vector<std::string> layer_names, 
                    std::vector<float> channel_means, 
                    int compute_unit_preference, 
                    int execution_data_type_preference, 
                    int latency,
					std::vector<std::string> input_layer_names){
						
    snap_options.compUnit = (SNAPInterface::SNAPCompUnit)compute_unit_preference;
    snap_options.cpuThreadCount = 1;
    snap_options.execType = (SNAPInterface::SNAPExecType)execution_data_type_preference;
    snap_options.model_file = file_path;
    snap_options.model_name = module_name;
	std::string model_extention = file_path.substr(file_path.rfind("."));
	LG<<"VZ Debug: set_snap_options model_extention= " << model_extention;
	std::map<std::string,SNAPInterface::SNAPModelType> mtype_map;
	mtype_map[".dlc"] = SNAPInterface::SNAPModelType::SNAP_SNPE;
	if(model_extention == ".dla" && (SNAPInterface::SNAPCompUnit)compute_unit_preference == SNAPInterface::SNAPCompUnit::SNAP_NPU){
		LG<<"VZ Debug: set_snap_options setting the model type to MNP";
		mtype_map[".dla"] = SNAPInterface::SNAPModelType::SNAP_MNP;
	}
	else if((SNAPInterface::SNAPCompUnit)compute_unit_preference == SNAPInterface::SNAPCompUnit::SNAP_NPU){
		LG<<"VZ Debug: set_snap_options setting the model type to EDEN";
		mtype_map[".tflite"] = SNAPInterface::SNAPModelType::SNAP_EDEN;
	}else if((SNAPInterface::SNAPCompUnit)compute_unit_preference == SNAPInterface::SNAPCompUnit::SNAP_GPU
			|| (SNAPInterface::SNAPCompUnit)compute_unit_preference == SNAPInterface::SNAPCompUnit::SNAP_CPU){
		LG<<"VZ Debug: set_snap_options setting the model type to SNAP_SNAPLITE";
		mtype_map[".tflite"] = SNAPInterface::SNAPModelType::SNAP_SNAPLITE;
	}
	mtype_map[".caffemodel"] = SNAPInterface::SNAPModelType::SNAP_CAFFE;
	mtype_map[".onnx"] = SNAPInterface::SNAPModelType::SNAP_ONNX;

	snap_options.mType = mtype_map[model_extention];

    snap_options.inputNames = input_layer_names;
	// for (int i=0;i<layer_names.size();i++)
	// 	LG<<"VZ Debug: set_snap_options input layername= " << input_layer_names[i];
	// for (int i=0;i<layer_names.size();i++)
	// 	LG<<"VZ Debug: set_snap_options layername= " << layer_names[i];
    snap_options.outputNames = layer_names;
    snap_options.weights_file = file_path;
	snap_options.cacheSize = latency;
	if((SNAPInterface::SNAPCompUnit)compute_unit_preference == SNAPInterface::SNAPCompUnit::SNAP_NPU && this->version != model::Version::V2_MULTI_FRAME){
		snap_options.mean = {128.0, 128.0, 128.0};
	    snap_options.scale = {1.0, 1.0, 1.0};
	}
	return ;
}


bool NNet::forward_pass(cv::Mat image){
	if(!open_status){
		LG<<"VZ Debug: Model not opened";
		return false;
	}
	if(image.empty()){
		LG<<"VZ Debug: Image for forward-pass cannot be empty";
		return false;
	}

	int width  = image.cols;
	int height = image.rows;
	int n_channels = image.channels();
	original_width = width;
	original_height = height;

	// Basic Image Pre-processing
	LOGV("VZ Debug forward_pass Target image size (CHW):[%d],[%d],[%d],n_channels:[%d] ", target_channels, target_height, target_width, n_channels);
	if(n_channels != target_channels){
		LG<<"VZ Debug: Expected "<<target_channels<<" channels in the input image, but received input image has "<<n_channels<<" channels";
		return false;
	}
	if(width != target_width || height != target_height){
		LG<<"VZ Debug: Miss-match in the input-image-size"<< "("<<width<<", "<<height<<")"<<" and the target-image-size"<< "("<<target_width<<", "<<target_height<<")"<<" in forward-pass";
		LG<<"VZ Debug: Resizing the image to match target input image size";
		cv::resize(image, image, cv::Size(target_width, target_height));
	}
	// if(prevFrame.empty()){
	// 	LG<<"VZ Debug: prevFrame.empty() True ";
	// 	prevFrame = image.clone();
	// }
	// std::vector<cv::Mat> target_mat_channels;
	// std::vector<cv::Mat> prev_target_mat_channels;
	cv::Mat target_image;
	// cv::split(image,target_mat_channels);
	// cv::split(prevFrame,prev_target_mat_channels);
	// target_mat_channels.insert(target_mat_channels.end(), prev_target_mat_channels.begin(), prev_target_mat_channels.end());
	// cv::merge(target_mat_channels, target_image);

	
	n_channels = 3;// TODO: Remove channel hardcoding 
	// image.convertTo(target_image, CV_32FC3);
	//target_image = normalize_image(image, this -> channel_means, this -> channel_scales);
	if(this->compute_unit != 3 || this->version == model::Version::V2_MULTI_FRAME) // Normalization not done on LSI model (NPU)(only done for multiframe LSI)
		target_image = normalize_image(image, this -> channel_means, this -> channel_scales);
	else{
		try{
			image.convertTo(target_image, CV_MAKE_TYPE(CV_32F, n_channels));
		}catch(std::exception e){
			LOGV("VZ Debug: Exception in in forward_pass : %s",e.what());
			return false;
		}catch(...){
			LG<<"VZ Debug: unknown exception in forward_pass";
			return false;
		}
	}
	

	float* image_buffer = (float*)target_image.data;
	if(!image_buffer){
        LG<<"VZ Debug: Image buffer for forward-pass cannot be empty";
        return false;
	}
	// Execute model on image
	int dli_image_format = (n_channels == 4) ? 2 : 1;
	dli_image_format = 1; //(n_channels == 6) ? 2 : 1;
	bool ret_value = false;
	LOGV("VZ Debug: NNet forward pass : execute");
	LOGV("VZ Debug: target_width = [%d], target_height=[%d] target_channels=[%d]", target_width, target_height, target_image.channels());
	
	if(snap_options.mType == SNAPInterface::SNAPModelType::SNAP_EDEN  && this->version == model::Version::V2_MULTI_FRAME)			//Multiframe LSI model takes NCHW raws as input
	{
		float* transposed_image = new float[3*target_width*target_height];
		int pixels = target_width*target_width;

		//if(1)//snap_options.mType == SNAPInterface::SNAPModelType::SNAP_ONNX){
		// auto start_time = CURRENT_TIME;
		if(target_width==512){
			LOGV("VZ Debug: converting to nchw for 512	");
			for(int i=0;i<pixels;i++){
				transposed_image[i]=image_buffer[i*3+0];
				transposed_image[i+pixels]=image_buffer[i*3+1];
				transposed_image[i+2*pixels]=image_buffer[i*3+2];
			}
			std::copy(transposed_image,transposed_image+(pixels*3),image_buffer);
		}
		delete []transposed_image;
		// LG<<"VZ Debug: Time taken for NHWC-NCHW: "<< CURRENT_TIME - start_time<<" ms";
	}
	#ifdef DUMP_INPUT
		std::string dumppath_input = "/sdcard/pdt_input_raws/" + std::to_string(imgcnt)+".raw";
		imgcnt++;
		std::ofstream out_debug(dumppath_input, std::ios::out | std::ios::binary);
		if(!out_debug) {
			LG<<"VZ Debug: Cannot open file."<<dumppath_input;
		} else {
			out_debug.write((char *) image_buffer, sizeof(float)*3*512*512);
			out_debug.close();
		}
		LG<<"VZ Debug: Dumped input success";
	#endif

	int start_time = CURRENT_TIME;
	

	if(this->version == model::Version::V2_MULTI_FRAME){
		auto DLIWrapper_multi_inp_execute_ptr = get_execute_multi_inp_fptr(dlinterface_lib_handle);

		// multi_input_buffers.clear();
		multi_input_buffers[0] = image_buffer;
		multi_input_buffers[1] = inter_state_128;
		multi_input_buffers[2] = inter_state_64;
		multi_input_buffers[3] = inter_state_32;
		multi_input_buffers[4] = inter_state_16;

		// LOGV("VZ Debug: NNet DLIWrapper_multi_inp_execute_ptr with new multi_input_buffers[%d] [0]=%p [1]=%p [2]=%p [3]=%p [4]=%p",multi_input_buffers.size(), multi_input_buffers[0], multi_input_buffers[1], multi_input_buffers[2], multi_input_buffers[3], multi_input_buffers[4]);
		if(open_status && DLIWrapper_multi_inp_execute_ptr (dli_handle, multi_input_buffers, multi_input_shapes)) {
			execute_status = true;
			ret_value = true;
		}else {
			execute_status = false;
			ret_value = false;
		}
	}else{
		auto DLIWrapper_execute_ptr = get_execute_fptr(dlinterface_lib_handle);
		if(open_status && DLIWrapper_execute_ptr (dli_handle, image_buffer, target_width, target_height, dli_image_format)) {
			execute_status = true;
			ret_value = true;
    	}
    	else {
    		execute_status = false;
    		ret_value = false;
    	}
	}

	// LOGV("VZ Debug: NNet forward pass execute_status=%d", execute_status);
	LG<<"VZ Debug: NNet forward pass execute_status="<<execute_status;
	/*
	auto DLIWrapper_execute_ptr = get_execute_fptr(dlinterface_lib_handle);
	if(open_status && DLIWrapper_execute_ptr (dli_handle, image_buffer, target_width, target_height, dli_image_format)) {
		execute_status = true;
		ret_value = true;
    }
    else {
    	execute_status = false;
    	ret_value = false;
    }
	*/
	// prevFrame = image.clone();
	//delete []transposed_image;
    LOGV("VZ Debug: NNet forward pass completed");
	int end_time = CURRENT_TIME;
	// LOGV("VZ Debug: forward pass completed engine-compute: Execute time taken: %d ms for %s-module", (end_time - start_time), module_name.c_str());
	LG<<"VZ Debug: forward pass completed engine-compute: Execute time taken:" << (end_time - start_time) << "ms for "<<module_name.c_str()<<"-module"; 
    return ret_value;
}

float* NNet::get_layer_output(const char* layerName, int &b, int &c, int &h, int &w) {
	if(!open_status){
		return NULL;
	}
	LOGV("VZ Debug: NNet get layer output called for %s", layerName);
	auto DLIWrapper_getLayerOutput_ptr = get_output_fptr(dlinterface_lib_handle);
	float* layer_output = NULL;
	if(execute_status) {
		layer_output = DLIWrapper_getLayerOutput_ptr(dli_handle, layerName, b, c, h, w);
		LOGV("VZ Debug: NNet layer complete output dimensions for %s are %d, %d, %d, %d", layerName, b, c, h, w);
	}
	if(!layer_output) {
		LOGV("VZ Debug: NNet get layer output NULL for %s", layerName);
		throw std::runtime_error("DLInterface: SNAP returned NULL for layerName " + std::string(layerName));
	}
	//LOGV("VZ Debug: NNet get layer output completed for %s", layerName);
	return layer_output;
}
float* NNet::get_layer_output_internal(const char* layerName, std::vector<int>& output_shape){
	if(!open_status){
		return NULL;
	}
	LOGV("VZ Debug: NNet get layer output shape called for %s", layerName);
	// LOGV("VZ Debug: NNet get layer output Returning dummy_output_for_debug = %p", dummy_output_for_debug);
	// output_shape.push_back(1);
	// output_shape.push_back(21824);
	// output_shape.push_back(39);
	// output_shape.push_back(1);
	// LOGV("VZ Debug: NNet get layer output Returning dummy_output_for_debug = %p, output_shape[%d]", dummy_output_for_debug, output_shape.size());
	// return dummy_output_for_debug;
	
	auto DLIWrapper_getLayerOutputShape_ptr = get_output_shape_fptr(dlinterface_lib_handle);
	float* layer_output = NULL;
	if(execute_status) {
		layer_output = DLIWrapper_getLayerOutputShape_ptr(dli_handle, layerName, output_shape);
		LOGV("VZ Debug: NNet layer complete output size for %s are %d, %d, %d, %d", layerName, output_shape[0], output_shape[1], output_shape[2], output_shape[3]);
	}
	if(!layer_output) {
		LOGV("VZ Debug: NNet get layer output NULL for %s", layerName);
		throw std::runtime_error("DLInterface: SNAP returned NULL for layerName " + std::string(layerName));
	}
	LOGV("VZ Debug: NNet get layer output completed for %s", layerName);
	return layer_output;

}

std::vector<float*> NNet::get_layer_multi_output_internal(const std::vector<char*> layerNames, std::vector<std::vector<int>>& output_shape){
	std::vector<float*> layer_output;
	if(!open_status){
		return layer_output;
	}
	LOGV("VZ Debug: NNet get layer multi output shape called for layerNames[%d], layerNames[0]=%s,layerNames[1]=%s,layerNames[2]=%s,layerNames[3]=%s,layerNames[4]=%s", layerNames.size(),layerNames[0], layerNames[1], layerNames[2],layerNames[3],layerNames[4]);
	auto DLIWrapper_getMultiLayerOutput_ptr = get_multi_layer_output_fptr(dlinterface_lib_handle);
	
	if(execute_status && DLIWrapper_getMultiLayerOutput_ptr) {
		layer_output = DLIWrapper_getMultiLayerOutput_ptr(dli_handle, layerNames, output_shape);
		// LOGV("VZ Debug: NNet layer complete output size for %s are %d, %d, %d, %d", layerName, output_shape[0], output_shape[1], output_shape[2], output_shape[3]);
	} else	
		return layer_output;
	
	LOGV("VZ Debug: NNet get layer multi output shape called for layerNames[%d], layerNames[0]=%s,layerNames[1]=%s,layerNames[2]=%s,layerNames[3]=%s,layerNames[4]=%s", layerNames.size(),layerNames[0], layerNames[1], layerNames[2],layerNames[3],layerNames[4]);
	if(layer_output.empty()) {
		LOGV("VZ Debug: NNet get layer output NULL for %s", layerNames[0]);
		throw std::runtime_error("DLInterface: SNAP returned NULL for layerName " + std::string(layerNames[0]));
	}
	LG<<"VZ Debug: NNet get layer output completed for "<<layerNames[0];
	return layer_output;

}

float* NNet::get_layer_output_shape(const char* layerName, std::vector<int>& output_shape) {
	//  "out_feat_16", "out_feat_32", "out_feat_64", "out_feat_128"};
	// std::vector<int> out_feat_16_shape;
	// std::vector<int> out_feat_32_shape;
	// std::vector<int> out_feat_64_shape;
	// std::vector<int> out_feat_128_shape;
	if(!open_status){
		return NULL;
	}
	float* layer_output = NULL;
	if(this->version == model::Version::V2_MULTI_FRAME){
		LOGV("VZ Debug: NNet get layer output shape called for Multi frame")
		std::vector<std::vector<int>> multi_shape;// {output_shape, out_feat_16_shape,out_feat_32_shape,out_feat_64_shape,out_feat_128_shape};
		// int start_time = CURRENT_TIME;

		// float* layer_output = get_layer_output_internal(layerName, output_shape); 
		// float* out_feat_16 = get_layer_output_internal("out_feat_16", out_feat_16_shape);
		// float* out_feat_32 = get_layer_output_internal("out_feat_32", out_feat_32_shape);
		// float* out_feat_64 = get_layer_output_internal("out_feat_64", out_feat_64_shape);
		// float* out_feat_128 = get_layer_output_internal("out_feat_128", out_feat_128_shape);
		// std::vector<char*> layer_names_cstr_internal;
		// for(auto layer_name_inter : output_layer_names_str){
		// 	layer_names_cstr_internal.push_back(const_cast<char*> (layer_name_inter.c_str()));
		// }
		// for(auto layer_name_inter : layer_names_cstr_internal){
		// 	LOGV("VZ Debug: NNet AUTO 2 get_layer_output_shape shape called for layerName = %s", layer_name_inter);

		// }

		std::vector<float*> multi_output = get_layer_multi_output_internal(layer_names_cstr, multi_shape);
		// int end_time = CURRENT_TIME;
		// LOGV("VZ Debug: get_layer_output_shape:  get_layer_output_internal Overhead Execute time taken: %d ms for %s-module", (end_time - start_time), module_name.c_str());

		// start_time = CURRENT_TIME;
		if(this->compute_unit==3)			//Order of outputs for Multiframe LSI model is different
		{
			std::memcpy(inter_state_16, multi_output[4], size_16);
			std::memcpy(inter_state_32, multi_output[3], size_32);
			std::memcpy(inter_state_64, multi_output[2], size_64);
			std::memcpy(inter_state_128, multi_output[1], size_128);
		}
		else
		{
			std::memcpy(inter_state_16, multi_output[1], size_16);
			std::memcpy(inter_state_32, multi_output[2], size_32);
			std::memcpy(inter_state_64, multi_output[3], size_64);
			std::memcpy(inter_state_128, multi_output[4], size_128);
		}
		// std::memcpy(inter_state_16, out_feat_16, size_16);
		// std::memcpy(inter_state_32, out_feat_32, size_32);
		// std::memcpy(inter_state_64, out_feat_64, size_64);
		// std::memcpy(inter_state_128, out_feat_128, size_128);
		// end_time = CURRENT_TIME;
		// LOGV("VZ Debug: get_layer_output_shape:  Copy Overhead Execute time taken: %d ms for %s-module", (end_time - start_time), module_name.c_str());
		
		// LOGV("VZ Debug: NNet get_layer_output_shape with new multi_output[%d] [0]=%f [1]=%f [2]=%f [3]=%f [4]=%f",multi_output.size(), multi_output[0], multi_output[1], multi_output[2], multi_output[3], multi_output[4]);
		// for(int i = 0; i< multi_shape.size(); i++){
		// 	for(int j = 0; j< multi_shape[i].size(); j++ ){
		// 		LOGV("VZ Debug: NNet get_layer_output_shape with new multi_shape[%d][%d]=%d",i,j,multi_shape[i][j]);
		// 	}
			
		// }
		// LOGV("VZ Debug: NNet get_layer_output_shape for %s layer_shape[%d]", layerName, multi_shape[0].size());
		// LOGV("VZ Debug: NNet get_layer_output_shape for %s layer_shape[%d]=%d,%d,%d,%d", layerName, multi_shape[0].size(), multi_shape[0][0], multi_shape[0][1], multi_shape[0][2], multi_shape[0][3] );
		
		if(this->compute_unit==3)			//Order of outputs for Multiframe LSI model is different
		{
			layer_output = multi_output[0];
			output_shape = multi_shape[0];
		}else{
			layer_output = multi_output[0];
			// LOGV("VZ Debug: NNet get_layer_output[0] = %f", layer_output[0]);
			// LOGV("VZ Debug: NNet get_layer_output[851135] = %f", layer_output[851135]);
			output_shape = multi_shape[0];
		}
	}else{
		LOGV("VZ Debug: NNet get layer output shape called for %s", layerName);
		auto DLIWrapper_getLayerOutputShape_ptr = get_output_shape_fptr(dlinterface_lib_handle);
		if(execute_status) {
			layer_output = DLIWrapper_getLayerOutputShape_ptr(dli_handle, layerName, output_shape);
			LOGV("VZ Debug: NNet layer complete output size for %s are %d, %d, %d", layerName, output_shape[0], output_shape[1], output_shape[2]);
		}
	}
	if(!layer_output) {
		LOGV("VZ Debug: NNet get_layer_output_shape NULL for %s", layerName);
		throw std::runtime_error("DLInterface: SNAP returned NULL for layerName " + std::string(layerName));
	}
	return layer_output; //get_layer_output_internal(layerName, output_shape);

	
}

float* NNet::get_layer_output_with_format(const char* layerName, int &b, int &c, int &h, int &w, int &data_format) {
	if(!open_status){
		return NULL;
	}
	LOGV("VZ Debug: NNet get layer output with format called for %s", layerName);
	auto DLIWrapper_getLayerOutputAndFormat_ptr = get_output_and_format_fptr(dlinterface_lib_handle);
	float* layer_output = NULL;
	if(execute_status) {
		layer_output = DLIWrapper_getLayerOutputAndFormat_ptr(dli_handle, layerName, b, c, h, w, data_format);
		// if(data_format == 1){ //nchw values are swapped in dlinterface
		// 	int t_c = c, t_h = h, t_w = w;
		// 	c = t_h;
		// 	h = t_w;
		// 	w = t_c;
		// }
		LOGV("VZ Debug: NNet layer complete output dimensions for %s are %d, %d, %d, %d and format: %d", layerName, b, c, h, w, data_format);
	}
	if(!layer_output) {
		LOGV("VZ Debug: NNet get layer output with format NULL for %s", layerName);
		throw std::runtime_error("DLInterface: SNAP returned NULL for layerName " + std::string(layerName));
	}
	return layer_output;
}


bool NNet::get_open_status(){
	return this->open_status;
}

void NNet::set_model_Version(model::Version version){
	this->version = version;
	//LG << "Model Version : "<< this->version;
}

NNet::~NNet() {
	int start_time = CURRENT_TIME;
	std::lock_guard<std::mutex> guard(deinit_mutex);
	if(init_thread.joinable()){
		LOGV("VZ Debug: Joining init thread in ~NNet for %s",this->module_name.c_str());
		init_thread.join();
	}

	if(time_fuse_thread.joinable()){
		time_fuse_thread.join();
	}
	delete[] inter_state_128;
    delete[] inter_state_64;
    delete[] inter_state_32;
    delete[] inter_state_16;
	// delete[] dummy_output_for_debug;

	auto DLIWrapper_destroy_ptr = get_destroy_fptr(dlinterface_lib_handle);
	LOGV("VZ Debug: NNet::~NNet: destroy without thread started");
	std::thread destroy_thread = std::move(std::thread(DLIWrapper_destroy_ptr, dli_handle));
	//DLIWrapper_destroy_ptr(dli_handle);
	if(destroy_thread.joinable()){
		destroy_thread.join();
	}
	LOGV("VZ Debug: NNet::~NNet: destroy without thread completed");
	dlclose(dlinterface_lib_handle);
	LOGV("VZ Debug: Closing of DLInterface handle completed in ~NNet for %s",this->module_name.c_str());
	int end_time = CURRENT_TIME;
	LOGV("VZ Debug: engine-compute: Close time taken: %d ms for module %s", (end_time - start_time), module_name.c_str());
}