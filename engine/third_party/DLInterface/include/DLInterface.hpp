#ifndef _DL_INTERFACE_HPP_
#define _DL_INTERFACE_HPP_

#include <memory>
#include <string>
#include <vector>

enum class DLInterfaceImageFormat { BGR = 1 };

class DLInterface final {
 public:
  DLInterface();
  ~DLInterface();
  bool openDLModel(const char* filePath, const char* moduleName,
                   const std::vector<std::string>& output_layer_names,
                   int compute_unit_preference, int execution_data_type_preference);
  bool openDLModel(const char* filePath, const std::vector<std::string>& output_layer_names, 
                int compute_unit_preference, int execution_data_type_preference);                   
  bool getInputDimens(int& width, int& height);
  bool getInputChannels(int& channels);
  bool setChannelMeans(const std::vector<float>& channel_means);
  bool execute(float* imageData, int width, int height,
               DLInterfaceImageFormat imageFormat);

  float* getLayerOutput(const char* layerName, int& b, int& c, int& h, int& w);
  float* getMultiLayerOutput(const std::vector<char*> layerNames, std::vector<std::vector<int>>& output_shape);
  bool getLayerInputInfo(const char* layerName, int& c, int& h, int& w);
  class DLInterfaceImpl;

 private:
  DLInterface(const DLInterface&) = delete;
  DLInterface& operator=(const DLInterface&) = delete;
  std::unique_ptr<DLInterfaceImpl> pimpl;
};

#endif
