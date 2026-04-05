/**
 * @file   sys_utils.hpp
 * @brief  This file provides common utilities
 *
 * This file provides interfaces for common utilities
 */

#ifndef SYS_UTILS
#define SYS_UTILS
#include <string>
#include <map>
#include "config.hpp"

/**
 * @brief Checks get the model version from system
 *
 * @return string representing the id
 */
std::string get_version_id_from_sys();
/**
 * @brief Checks if processor is exynos
 *
 * @return bool true is exynos, false otherwise
 */
bool is_chipset_exynos();
/**
 * @brief Gets the model version
 * 
 * @param[in] module_name representing modulename
 * @param[in] base_folder representing the model location
 *
 * @return string representing model version
 */
std::string get_model_version(std::string module_name, std::string base_folder,vizpet::PetDetector::Mode mode);
/**
 * @brief Gets the properties of the model
 * 
 * @param[in] module_name representing modulename
 * @param[in] base_folder representing the model location
 *
 * @return map representing model properties
 */
std::map<std::string, std::string> get_model_properties(std::string module_name, std::string base_folder,vizpet::PetDetector::Mode mode);

int get_custom_adb_int_val(std::string keyword);
std::string get_custom_adb_str_val(std::string keyword);

/**
 * @brief Checks of file existings in a path
 * 
 * @param[in] file path
 *
 * @return bool true if file exists, false otherwise
 */
bool file_exists(std::string file_path);
std::string get_final_models_base_path(std::string base_path, std::string module_name);

/**
 * @brief Gets the extension of the model
 * 
 * @param[in] compute unit
 *
 * @return string containing the model extension
 */
std::string get_model_file_extension(snap::ComputeUnit compute_unit);
/**
 * @brief Checks if model is present in base/system/vendor path based on compute unit
 * 
 * @param[in] base path
 * @param[in] module name
 * @param[in] compute unit
 *
 * @return string containing the model path
 */
std::string get_final_model_path(std::string base_path, std::string module_name, snap::ComputeUnit compute_unit);
/**
 * @brief Checks if model is present in base/system/vendor path based on compute unit
 * 
 * @param[in] base path
 * @param[in] module name
 * @param[in] compute unit number
 *
 * @return string containing the model path
 */
std::string get_final_model_path(std::string base_path, std::string module_name, int compute_unit_number);

#endif