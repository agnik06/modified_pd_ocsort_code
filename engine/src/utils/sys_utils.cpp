/**
 * @file   sys_utils.cpp
 * @brief  This file provides common utilities
 *
 * This file provides interfaces for common utilities
 */

#include "sys_utils.hpp"
#ifdef __android__
#include <sys/system_properties.h>
#endif
#include "logger/logger.hpp"
#include "json.hpp"
#include <fstream>
#include <sys/stat.h>

std::string get_version_id_from_sys() {
	std::string version_id = "v3";
	#ifdef __android__
	char device[PROP_VALUE_MAX + 1];
	__system_property_get("ro.product.device", device);
	std::string device_str(device);
	#ifdef DUMPBOX
		LOGV("VZ Debug: System property ro.product.device %s", device_str.c_str());
	#endif
	if(device_str.find("crown") != std::string::npos) {
		version_id = "v3";
	}
	else {
		version_id = "v4";
	}
	#endif
	#ifdef DUMPBOX
		LOGV("VZ Debug: version_id set as %s", version_id.c_str());
	#endif
	return version_id;
}

bool is_chipset_exynos() {
#ifdef __android__
	char chipname[PROP_VALUE_MAX + 1];
	__system_property_get("ro.hardware.chipname", chipname);
	std::string chipname_str(chipname);
	LOGV("VZ Debug: Chipset name - %s", chipname_str.c_str());
	return (chipname_str.find("exynos") != std::string::npos);
#else
	return false;
#endif
}

std::string get_model_version(std::string module_name, std::string base_folder, vizpet::PetDetector::Mode mode){
	std::string version_id = "V1";
	nlohmann::json json_data;

	base_folder = get_final_models_base_path(base_folder, module_name);

	std::string meta_filename;
	if(mode == vizpet::PetDetector::Mode::SALIENCY){
		meta_filename = base_folder + std::string(module_name) + "/" + std::string(module_name) + "_cnn.info";
		#ifdef DUMPBOX
			LOGV("VZ Debug: get_get_model_versionmodel_properties info file path is %s, mode: %d",meta_filename.c_str(), mode);
		#endif
	}else{
		meta_filename = base_folder + std::string(module_name) + "/" + std::string(module_name) + "_cnn_system.info";
		#ifdef DUMPBOX
			LOGV("VZ Debug: get_model_version info file path is %s, mode: %d",meta_filename.c_str(), mode);
		#endif
	}
	std::ifstream infile(meta_filename);

	if (!base_folder.empty() && infile.good()) {
		infile >> json_data;
		version_id = json_data["version_id"];
		#ifdef TESTING
		#ifdef DUMPBOX
		LOGV("VZ Debug: Loading model version from %s, version_id: %s",meta_filename.c_str(), version_id.c_str());
		#endif
		#endif
		infile.close();
	}
	else{
		#ifdef DUMPBOX
			LOGV("VZ Debug: Loading model version from model info file failed, default version_id: %s", version_id.c_str());
		#endif
	}
	return version_id;
}

std::map<std::string, std::string> get_model_properties(std::string module_name, std::string base_folder,vizpet::PetDetector::Mode mode){
	std::map<std::string, std::string> config_map;
		LOGV("VZ Debug: get_model_properties module_name is %s, base_folder : %s",module_name.c_str(), base_folder.c_str());
	std::string version_id = "v5";
	std::string model_type = "cpu";
	std::string precision = "float16";
	std::string model_path = "";
	std::string compiler = "";
	std::string latency = "0";
	std::string input_layer = "data";
	std::string output_layer = "output";
	std::string resolution = "384";

	nlohmann::json json_data;

	base_folder = get_final_models_base_path(base_folder, module_name);
	std::string meta_filename;
	if(mode == vizpet::PetDetector::Mode::SALIENCY){
		meta_filename = base_folder + std::string(module_name) + "/" + std::string(module_name) + "_cnn.info";
			LOGV("VZ Debug: get_model_properties info file path is %s, mode: %d",meta_filename.c_str(), mode);
	}else{
		meta_filename = base_folder + std::string(module_name) + "/" + std::string(module_name) + "_cnn_system.info";
			LOGV("VZ Debug: get_model_properties info file path is %s, mode: %d",meta_filename.c_str(), mode);
	}
	std::ifstream infile(meta_filename);
	if(!base_folder.empty() && infile.good()){
		infile >> json_data;
		try{
			version_id = json_data["version_id"];
		}catch(std::exception e){
			#ifdef DUMPBOX
				LOGV("VZ Debug: Exception in reading from version id from file : %s",e.what());
			#endif
		}catch(...){
			LG<<"VZ Debug: unknown exception while reading version id from file";
		}
		try{
			model_type = json_data["model_type"];
		}catch(std::exception e){
			#ifdef DUMPBOX
				LOGV("VZ Debug: Exception in reading from model type from file : %s",e.what());
			#endif
		}catch(...){
			LG<<"VZ Debug: unknown exception while reading model type from file";
		}
		try{
			precision  = json_data["precision"];
		}catch(std::exception e){
			#ifdef DUMPBOX
				LOGV("VZ Debug: Exception in reading from precision from file : %s",e.what());
			#endif
		}catch(...){
			LG<<"VZ Debug: unknown exception while reading precision from file";
		}
		try{
			compiler = json_data["compiler"];
		}catch(std::exception e){
			#ifdef DUMPBOX
				LOGV("VZ Debug: Exception in reading from compiler from file : %s",e.what());
			#endif
		}catch(...){
			LG<<"VZ Debug: unknown exception while reading compiler from file";
		}
		try{
			std::string tag = json_data["tag"];
			LG<<"VZ Debug:model tag:"<<tag;
		}catch(std::exception e){
			#ifdef DUMPBOX
				LOGV("VZ Debug: Exception in reading from compiler from file : %s",e.what());
			#endif
		}catch(...){
			LG<<"VZ Debug: unknown exception while reading compiler from file";
		}
		try{
			model_path  = json_data["model_path"];
		}catch(std::exception e){
			#ifdef DUMPBOX
				LOGV("VZ Debug: \"model_path\" could not be found in the meta file");
			#endif
		}catch(...){
			LG<<"VZ Debug: unknown exception while reading model_path from meta file";
		}

		try{
			latency  = json_data["latency"];
		}catch(std::exception e){
			#ifdef DUMPBOX
				LOGV("VZ Debug: latency could not be found in the meta file");
			#endif
		}catch(...){
			LG<<"VZ Debug: unknown exception while reading latency from meta file";
		}

		if(json_data.contains("input_layer")){
			try{
				LOGV("VZ Debug: input_layer ")
				input_layer  = json_data["input_layer"];
			}catch(std::exception e){
				input_layer="data";
				LOGV("VZ Debug: input_layer could not be found in the meta file");
			}catch(...){
				input_layer="data";
				LG<<"VZ Debug: unknown exception while reading input_layer from meta file";
			}
		}
		if(json_data.contains("output_layer")){
			try{
				output_layer  = json_data["output_layer"];
			}catch(std::exception e){
				output_layer = "output";
				LOGV("VZ Debug: output_layer could not be found in the meta file");
			}catch(...){
				output_layer = "output";
				LG<<"VZ Debug: unknown exception while reading output_layer from meta file";
			}
		}
		if(json_data.contains("resolution")){
			try{
				resolution  = json_data["resolution"];
			}catch(std::exception e){
				resolution="384";
				LOGV("VZ Debug: resolution could not be found in the meta file");
			}catch(...){
				resolution="384";
				LG<<"VZ Debug: unknown exception while reading resolution from meta file";
			}
		}
		try
		{
			infile.close();
		}
		catch(const std::exception& e)
		{
			LOGV("VZ Debug: Exception in reading from info file : %s",e.what());
		}
		
		
		
		// #ifdef TESTING
			LOGV("VZ Debug: Loading model properties from %s, model properties: %s, %s",meta_filename.c_str(), model_type.c_str(), precision.c_str());
			LOGV("VZ Debug: Loading model properties from %s, model properties: input_layer : %s, output_layer: %s, resolution: %s",meta_filename.c_str(), input_layer.c_str(), output_layer.c_str(), resolution.c_str());
		// #endif
		LG<<"Compiler version:"<<compiler.c_str();
	}
	else{
		// #ifdef TESTING
			LOGV("VZ Debug: Loading model properties from model for %s info file failed, default model properties: %s, %s",meta_filename.c_str(), model_type.c_str(), precision.c_str());
		// #endif
	}
	config_map["version_id"] = version_id;
	config_map["model_type"] = model_type;
	config_map["precision"] = precision;
	config_map["model_path"] = model_path;
	config_map["latency"] = latency;
	config_map["input_layer"] = input_layer;
	config_map["output_layer"] = output_layer;
	config_map["resolution"] = resolution;
	config_map["compilier"] = compiler;
	return config_map;
}


int get_custom_adb_int_val(std::string keyword)
{
	int iCustum_val = 0;
#ifdef TESTING
#ifdef __android__
	char word_by_key[PROP_VALUE_MAX + 1] = {};
	__system_property_get(keyword.c_str(), word_by_key);
	std::string custom_str(word_by_key);
	if (custom_str.c_str() != NULL)
		iCustum_val = atoi(custom_str.c_str());
	#ifdef DUMPBOX
		LOGV("Moon Debug: get_custom_adb [%s] %d", keyword.c_str(), iCustum_val);
	#endif
#endif
#endif
	return iCustum_val;
}
std::string get_custom_adb_str_val(std::string keyword)
{
	std::string custom_str = "";
#ifdef TESTING
#ifdef __android__
	char word_by_key[PROP_VALUE_MAX + 1] = {};
	__system_property_get(keyword.c_str(), word_by_key);
	custom_str = word_by_key;
	#ifdef DUMPBOX
		LOGV("Moon Debug: get_custom_adb [%s] %s", keyword.c_str(), custom_str.c_str());
	#endif
#endif
#endif
	return custom_str;
}

bool file_exists(std::string file_path){
	#ifdef DUMPBOX
		LOGV("VZ Debug: file_exists file_path is %s",file_path.c_str());
	#endif
	struct stat info;
	if(stat(file_path.c_str(), &info) == 0){
		return true;
	}
	std::ifstream infile(file_path.c_str());
	bool file_good = infile.good();
	infile.close();
	return file_good;
}

//base path contains 'segmenter', 'detector', etc., directories
std::string get_final_models_base_path(std::string base_path, std::string module_name){
	#ifdef DUMPBOX
		LOGV("VZ Debug: get_final_models_base_path base_path is %s, module_name : %s",base_path.c_str(), module_name.c_str());
	#endif
	std::string vendor_path = "/vendor/saiv/image_understanding/db/";
	std::string system_path = "/system/saiv/image_understanding/db/";
	std::string final_base_path = "";

    if(file_exists(base_path + module_name)){
		final_base_path = base_path;
	}
	else if(file_exists(system_path + module_name)){
    	final_base_path = system_path;
    }
	else if(file_exists(vendor_path + module_name)){
		final_base_path = vendor_path;
	}
// #ifdef TESTING
	if(final_base_path.empty()){
		#ifdef DUMPBOX
			LOGV("VZ Debug: Base model parent path not found for %s", module_name.c_str());
		#endif
	}
	else{
		#ifdef DUMPBOX
			LOGV("VZ Debug: Base model parent path: %s for %s", final_base_path.c_str(), module_name.c_str());
		#endif
	}
// #endif
	return final_base_path;
}