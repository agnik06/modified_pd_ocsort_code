/**
 * @file   roi_data_handler.hpp
 * @brief  This file provides utilities for roi rectagle box
 *
 * This file provides utilities for roi rectagle box
 */

#ifndef __ROI_DATA_HANDLER__
#define __ROI_DATA_HANDLER__

#include "json.hpp"
#include <string>
#include <map>
#include <vector>

using json = nlohmann::json;
class ROIDataHandler{
public:
    ROIDataHandler();
    ROIDataHandler(json json_data);
    ~ROIDataHandler();

    //gets the label from the id
    std::string get_label(int id);
    //get the priority based on id
    int get_priority(int id);
    //get the priority based on label name
    int get_priority(std::string label);
    //check if id is valid
    bool is_valid_id(int id);
    //get the threshold based on label name
    float get_threshold(std::string label);
    //get the cluster label based on label name
    std::string get_cluster_label(std::string label);
    //get the cluster label for the id
    std::string get_cluster_label(int id);

private:
    std::map<std::string, int>  label_id_map;
    std::map<int, std::string>  id_label_map;
    std::map<std::string, float> thresholds;
    std::map<std::string, std::string> type;
    std::map<std::string, std::string> cluster_label;
    std::map<std::string, int> priority_index;

    void initialize(json json_str);
};

#endif //__ROI_DATA_HANDLER__