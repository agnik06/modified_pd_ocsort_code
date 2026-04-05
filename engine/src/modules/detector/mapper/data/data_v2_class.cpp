/**
 * @file   data_v1_class.cpp
 * @brief  This file defines the thresholds for different classes in v2
 *
 * This provides the thresholds for different classes in v2
 * Classes : object, person, food, vehicle, flower, animal, drink, petface, moon
 */

#include "roi_data_factory.hpp"
#include "json.hpp"
nlohmann::json get_json_str_v2(){

nlohmann::json detector_json_str_V2 = R"(
{
  "roi_tags": [
    {
      "label": "background", 
      "id": 0, 
      "threshold": 1.0,
      "type": "BG",
      "cluster_label": "background", 
      "priority_index": 0
    },
    {
      "label": "Object", 
      "id": 1, 
      "threshold": 1.0,
      "type": "BG",
      "cluster_label": "background", 
      "priority_index": 0
    },
    {
      "label": "person",
      "id": 2,
      "threshold": 0.285,
      "type": "FG",
      "cluster_label": "alive",
      "priority_index": 1
    },
    {
      "label": "petface",
      "id": 3,
      "threshold": 0.40,
      "type": "FG",
      "cluster_label": "alive",
      "priority_index": 1
    },
    {
      "label": "human_head",
      "id": 4,
      "threshold": 0.3,
      "type": "FG",
      "cluster_label": "alive",
      "priority_index": 1
    },
    {
      "label": "pets",
      "id": 5,
      "threshold": 0.40,
      "type": "FG",
      "cluster_label": "alive",
      "priority_index": 1
    },
    {
      "label": "PetEyes",
      "id": 6,
      "threshold": 0.20,
      "type": "FG",
      "cluster_label": "alive",
      "priority_index": 1
    }
  ]
}
)"_json;
return detector_json_str_V2;
}
