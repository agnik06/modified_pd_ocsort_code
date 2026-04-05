/**
 * @file   config.cpp
 * @brief  Defines the config for object detector
 *
 * Defines the configuration for the object detector
 */

#include "config.hpp"
#include "utils/sys_utils.hpp"
#include "logger/logger.hpp"

// std::shared_ptr<Config> Config::m_config = 0;
Config::Config(){
}

void Config::setMode(vizpet::PetDetector::Mode mode){
	this->mode = mode;
}
vizpet::PetDetector::Mode Config::getMode(){
	return this->mode;
}
std::string Config::getModeAsString(){
	switch(this->mode){
		case vizpet::PetDetector::Mode::SALIENCY:	return "SALIENCY";
	}
	return "";
}

void Config::addModule(ModuleProperty module){
	this->modules.push_back(module);
}
void Config::clearModules(){
	this->modules.clear();
}
std::vector<ModuleProperty> Config::getModules(){
	return this->modules;
}

void Config::setModelFilePath(const char* path){
	this->model_file_path = path;
}
void Config::setModelFilePath(std::string path){
	this->model_file_path = path;
}
std::string Config::getModelFilePath(){
	return this->model_file_path;
}

model::Version Config::getVersion(){
	return this->version;
}

void Config::setVersion(model::Version version){
	this->version = version;
}

void Config::setVersionFromModule(std::string module_name){
	// std::string model_version_id = get_model_version(module_name, Config::Instance()->getModelFilePath());
	std::string model_version_id = get_model_version(module_name, this->getModelFilePath(),this->getMode());
	if(model_version_id == "V1") version = model::Version::V1;
	else if(model_version_id == "V2") version = model::Version::V2;
	else if(model_version_id == "V2_MULTI_FRAME") version = model::Version::V2_MULTI_FRAME;
	
}

// std::shared_ptr<Config> Config::Instance(){
// 	if(!m_config){
// 			m_config.reset(new Config());
// 	}
// 	return m_config;
// }


ModuleProperty::ModuleProperty(std::string module_name, std::shared_ptr<Config> config){
	this->name = module_name;
	LOGV("VZ Debug: setting COMPUTE UNIT to %s", this->name.c_str());
	// std::map<std::string, std::string> config_map = get_model_properties(this->name, Config::Instance()->getModelFilePath());
	//std::map<std::string, std::string> config_map = get_model_properties(this->name, config->getModelFilePath());
	std::map<std::string, std::string> config_map = get_model_properties(this->name, config->getModelFilePath(), config->getMode());

	LOGV("VZ Debug: setting COMPUTE UNIT to %s, %s", config_map["model_type"].c_str(), config_map["precision"].c_str());

	if(config_map["model_type"] == "cpu"){
		this->compute_unit = snap::ComputeUnit::CPU;
	}else if(config_map["model_type"] == "gpu"){
		this->compute_unit = snap::ComputeUnit::GPU;
	}else if(config_map["model_type"] == "dsp"){
		this->compute_unit = snap::ComputeUnit::DSP;
	}else if(config_map["model_type"] == "npu"){
		this->compute_unit = snap::ComputeUnit::NPU;
	}else{
		LOGV("VZ Debug: Couldn't identify the COMPUTE UNIT from .info file, so setting COMPUTE UNIT to CPU");
		this->compute_unit = snap::ComputeUnit::CPU;
	}

	if(config_map["precision"] == "float16"){
		this->execution_type = snap::ExecutionDataType::FLOAT16;
	}else if(config_map["precision"] == "float32"){
		this->execution_type = snap::ExecutionDataType::FLOAT32;
	}else if(config_map["precision"] == "qasymm8"){
		this->execution_type = snap::ExecutionDataType::QASYMM8;
	}else if(config_map["precision"] == "qasymm16"){
		this->execution_type = snap::ExecutionDataType::QASYMM16;
	}else{
		LOGV("VZ Debug: Couldn't identify the EXECUTION DATA TYPE from .info file, so setting EXECUTION DATA TYPE to FLOAT16");
		this->execution_type = snap::ExecutionDataType::FLOAT16;
	}
	this->latency = std::stoi(config_map["latency"]);
	this->model_path = config_map["model_path"];
	this->input_layer = config_map["input_layer"];
	this->output_layer = config_map["output_layer"];
	this->resolution = std::stoi(config_map["resolution"]);
	this->compilier = config_map["compilier"];
}
ModuleProperty::ModuleProperty(std::string name, snap::ComputeUnit compute_unit, snap::ExecutionDataType execution_type){
	this->name = name;
	this->compute_unit = compute_unit;
	this->execution_type = execution_type;
	this->model_path = "";
}