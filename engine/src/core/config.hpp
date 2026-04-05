/**
 * @file   config.hpp
 * @brief  Defines the config for object detector
 *
 * Defines the configuration for the object detector
 */

#ifndef __CONFIG__
#define __CONFIG__

#include <string>
#include <memory>
#include <vector>

#include "pet_detector.h"
#include "model_config.hpp"

namespace snap {
	/**
	 * Defines the Compute unit for the NN model
	 */
	enum class ComputeUnit {
					CPU,
					GPU,
					DSP,
					NPU,
					MAXENUM,
	};

	/**
	 * Defines the Execution datatype for compute unit
	 */
	enum class ExecutionDataType {
					FLOAT32,
					FLOAT16,
					QASYMM16,
					QASYMM8,
					MAXENUM,
					/*Revisit: Add more error codes*/
	};
}

class Config;
/**
 * ModuleProperty stores the model name, computeunit, exec unit type,  latency info, model path
 */
class ModuleProperty{
public:
	std::string name;
	snap::ComputeUnit compute_unit;
	snap::ExecutionDataType execution_type;
	int latency;
	int resolution;
	std::string input_layer; 
	std::string output_layer; 
	std::string model_path;
	std::string compilier;
	ModuleProperty(std::string module_name, std::shared_ptr<Config> config);
	ModuleProperty(std::string name, snap::ComputeUnit compute_unit, snap::ExecutionDataType execution_type);
};

/**
 * Config maintains differnt kinds of modes, get and set attributes, model path
 */
class Config{
private:
	// static std::shared_ptr<Config> m_config;//Singleton
	vizpet::PetDetector::Mode mode = vizpet::PetDetector::Mode::SALIENCY;
	std::string model_file_path = "/system/saiv/image_understanding/db/";
	std::vector<ModuleProperty> modules;
	model::Version version = model::Version::V1;

public:
	Config();

	void setVersion(model::Version version);
	void setVersionFromModule(std::string module_name);
	model::Version getVersion();

	void setMode(vizpet::PetDetector::Mode mode);
	vizpet::PetDetector::Mode getMode();
	std::string getModeAsString();
	
	void addModule(ModuleProperty module);
	void clearModules();
	std::vector<ModuleProperty> getModules();

	void setModelFilePath(const char* path);
	void setModelFilePath(std::string path);
	std::string getModelFilePath();

	// static std::shared_ptr<Config> Instance();//Singleton
};
#endif //__CONFIG__