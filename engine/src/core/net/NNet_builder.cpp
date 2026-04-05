#include "NNet.hpp"
#include "logger/logger.hpp"
#include <memory>
#include <assert.h>

NNet::Builder::Builder(){
    model_path = "";
    module_name = "";
    // output_layer_names;
    compute_unit = -1;
    execution_data_type = -1;
    // channel_means;
    init_callback_fn = NULL;
}
NNet::Builder& NNet::Builder::set_model_path(std::string model_path){
    this->model_path = model_path;
    return *this;
}
NNet::Builder& NNet::Builder::set_module_name(std::string module_name){
    this->module_name = module_name;
    return *this;    
}
NNet::Builder& NNet::Builder::set_output_layer_names(std::vector<std::string> output_layer_names){
    this->output_layer_names = output_layer_names;
    return *this;
}
NNet::Builder& NNet::Builder::set_compute_unit(int compute_unit){
    this->compute_unit = compute_unit;
    return *this;
}
NNet::Builder& NNet::Builder::set_execution_data_type(int execution_data_type){
    this->execution_data_type = execution_data_type;
    return *this;
}
NNet::Builder& NNet::Builder::set_latency(int _latency){
    this->latency = _latency;
    return *this;
}
NNet::Builder& NNet::Builder::set_input_layer_names(std::vector<std::string> input_layer_names){
    this->input_layer_names = input_layer_names;
    return *this;
}
NNet::Builder& NNet::Builder::set_resolution(int _resoltion){
    this->resolution = _resoltion;
    return *this;
}
NNet::Builder& NNet::Builder::set_channel_means(std::vector<float> means){
    this->channel_means = means;
    return *this;
}
NNet::Builder& NNet::Builder::set_init_callback_fn(NNetInitCallback callback_fn){
    this->init_callback_fn = callback_fn;
    return *this;
}

NNet::Builder& NNet::Builder::validate(){
    assert(model_path != ""     && "Error: Expected a valid \'model_path\' parameter");
    assert(module_name != ""    && "Error: Expected a valid \'module_name\' parameter");
    assert(!output_layer_names.empty() && "Error: Expected atleast 1 output layer name");
    assert(compute_unit != -1   && "Error: Expected a valid \'compute_unit\' parameter");
    assert(execution_data_type != -1   && "Error: Expected a valid \'execution_data_type\' parameter");

    if(channel_means.size() == 0){
        LG<<"Warning: Default channels means (n=3) will be used, number of input channels is set to 3";
    }
    if(init_callback_fn == NULL){
        LG<<"Warning: Models will be loaded sequentially as there is no callback_fn";
    }
    return *this;
}

std::shared_ptr<NNet> NNet::Builder::build(){
    validate();
    return std::make_shared<NNet>(model_path, module_name, output_layer_names, channel_means, compute_unit, execution_data_type,latency, input_layer_names, resolution, init_callback_fn);
}

NNet::NNet(NNet::Builder& builder) :
    NNet::NNet(builder.model_path, builder.module_name, builder.output_layer_names, builder.channel_means ,builder.compute_unit, builder.execution_data_type,builder.latency, builder.input_layer_names, builder.resolution, builder.init_callback_fn){
}
