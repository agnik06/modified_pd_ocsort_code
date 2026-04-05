/**
 * @file   tagger_factory.hpp
 * @brief  This file provides base for the tagger
 *
 * This class provides the details for the tagger
 */

#ifndef __TAGGER_FACTORY__
#define __TAGGER_FACTORY__

#include "pet_detector.h"
#include "tagger/base_tagger.hpp"
#include "config.hpp"
#include <memory>
#include <functional>
#include "model_config.hpp"


typedef std::function<void(bool)> TaggerInitCallback;

class TaggerFactory{
public:
    TaggerFactory();
    ~TaggerFactory();
    /**
     * @brief Initializes the tagger and get instance for detection
     *
     * @param[in] config for the tagger
     * @param[in] tagger callback to initialize with
     *
     * @return Instance of the taggger which is used subsequently
     */
   std::shared_ptr<BaseTagger> get_tagger(std::shared_ptr<Config> config, TaggerInitCallback tagger_init_cb=NULL);

private:
    std::map<std::string, int> tagger_module_init_status;
    std::string get_model_path(std::string base_folder, std::string module_name, std::string model_extension);
    std::tuple<int, int, int, std::string, std::string, std::string, int> get_snap_config(std::shared_ptr<Config> config, std::string module_name);
};

#endif //__TAGGER_FACTORY__