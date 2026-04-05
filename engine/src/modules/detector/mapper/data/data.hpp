/**
 * @file   data.hpp
 * @brief  This file defines the tag parameters all the LOD classes
 *
 * This file defines the tag parameters for 6 class
 * Classes : object, person, food, vehicle, flower, animal
 */

#ifndef __L_DETECTOR_DATA__
#define __L_DETECTOR_DATA__

#include "json.hpp"
nlohmann::json detector_json_str = R"(
{
  "roi_tags": [
    {
      "label": "background", 
      "id": 0, 
      "threshold": 0.0,
      "type": "BG",
      "cluster_label": "background", 
      "priority_index": 0
    },
    {
      "label": "object", 
      "id": 1, 
      "threshold": 0.0,
      "type": "FG",
      "cluster_label": "object", 
      "priority_index": 1
    }, 
    {
      "label": "person", 
      "id": 2, 
      "threshold": 0.0,
      "type": "FG",
      "cluster_label": "person", 
      "priority_index": 1
    }, 
    {
      "label": "food", 
      "id": 3, 
      "threshold": 0.0,
      "type": "FG",
      "cluster_label": "head", 
      "priority_index": 1
    },
    {
      "label": "vehicle", 
      "id": 4, 
      "threshold": 0.0,
      "type": "FG",
      "cluster_label": "pet", 
      "priority_index": 1
    },
    {
      "label": "flower", 
      "id": 5, 
      "threshold": 0.0,
      "type": "FG",
      "cluster_label": "pet", 
      "priority_index": 1
    },
    {
      "label": "animal", 
      "id": 6, 
      "threshold": 0.0,
      "type": "FG",
      "cluster_label": "pet", 
      "priority_index": 1
    }
  ]
}
)"_json;

#endif //__DETECTOR_DATA__