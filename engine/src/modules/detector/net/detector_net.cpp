/**
 * @file   detector_net.cpp
 * @brief  This file implements the detection network
 *
 * This file implements  the outputs based on layer name
 */
#include "net/NNet.hpp"
#include "detector_net.hpp"

#include "logger/timer.hpp"
#include "nms_ssd.hpp"
#include "decoder_ssd.hpp"
#include <iostream>
#include <fstream>
int num_priors = 8190;

//#define SNAP_ORDER_FIX
#ifdef TESTING
#define DUMP_OUTPUT (1)
#endif
int imgcnt=0;

#include <sys/stat.h>

long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


bool detector_dump_outputlayer(float* buffer, int b, int c, int h, int w)
{
	LG << "detector_dump_outputlayer b:" << b << "c:" << c << "h:" << h << "w:" << w;
#ifdef __android__
    //dump output
    std::string dumppath = "/sdcard/pet_detector_raws/" + std::to_string(imgcnt)+".raw";
#else
    std::string dumppath = "/data/venkat/pet_detector_raws/" + std::to_string(imgcnt)+".raw"; //in 21675
#endif
	imgcnt++;
	LG << "Dump path:: " << dumppath.c_str();
    std::ofstream out(dumppath, std::ios::out | std::ios::binary  | std::ios::ate);
    if(!out) {
        LG << "detector_dump_outputlayer Cannot open file." << dumppath.c_str();
        return 0;
    }
    //cout<<"size is  "<<sizeof(vals)<<endl;
	long size =  sizeof(float)*b*c*h*w;
  LG << "detector_dump_outputlayer Writing to file size: " << size;
  out.write((char *) buffer, size);
  out.close();
  LG << "detector_dump_outputlayer size: " << size;
  long sizewritten = GetFileSize(dumppath);
  LG << "detector_dump_outputlayer size written: " << sizewritten;

  return 1;
}

float* get_snap_raw_buffer(std::vector<int>& output_shape){
	int c = output_shape[0];
	int h = output_shape[1];
	int w = output_shape[2];
	int size = c*h*w*sizeof(float);
	LG<<"reading from file size=" << size;
	std::string raw_path = "/sdcard/0.jpg.raw";
	std::ifstream in(raw_path.c_str(),std::ios::in| std::ios::binary);
	if(!in) {
		LG << "get_snap_raw_buffer Cannot open file." << raw_path.c_str();
		return 0;
	}

	float* flat_output = new float(size);
	LG << "get_snap_raw_buffer Cannot open file." << raw_path.c_str();
	in.read((char*)flat_output, size);
	LG << "get_snap_raw_buffer Cannot open file." << raw_path.c_str();
	in.close();
	long sizewritten = GetFileSize(raw_path);
	LG << "get_snap_raw_buffer size written: " << sizewritten;
	return flat_output;
}

DetectorNet::DetectorNet(NNet::Builder& builder,int _target_width,int _target_height, model::Version version) : NNet(builder){
    m_decoder = new Decoder(_target_width,_target_height, version);
	set_model_Version(version);
	//m_decoder->set_target_dimen(_target_width, _target_height);
}

DetectorNet::~DetectorNet() {
	if(m_decoder){
		delete m_decoder;
		m_decoder = 0;
	}
}

/**
 * @brief gets the output based on layer name
 * @details Gets the output of last layer
 *          c-> channel, h->height, w->width
 *          There are diferent formats of outputs chw and hwc
 *          LSI and SNPE have same output formats as of now.
 *          Output Results do vary if the formats are changed.
 *
 * @param[in] output layername
 *
 * @return tensor with layer details
 */
Tensor3D<float> DetectorNet::get_output(const char* layerName){
	int b, c, h, w, data_format;
	//float *layer_output_flattened = get_layer_output_with_format(layerName, b, c, h, w, data_format);
	float *layer_output_flattened = get_layer_output(layerName, b, c, h, w);

#ifdef DUMP_OUTPUT
	bool dumpresult = detector_dump_outputlayer(layer_output_flattened, b,c,h,w);
	if(dumpresult){
		#ifdef DUMPBOX
			LOGV("VZ Debug: DetectorNet::get_output : dump success");
		#endif
	}else{
		#ifdef DUMPBOX
			LOGV("VZ Debug: DetectorNet::get_output : dump failed");
		#endif
	}
#endif
#ifndef SNAP_ORDER_FIX
	Tensor3D<float> output_tensor(layer_output_flattened, c, h, w);
#else
	Tensor3D<float> output_tensor(layer_output_flattened, h, w, c);
#endif
	return output_tensor;
}

std::vector<std::vector<float> > DetectorNet::get_output_custom_decode_with_nms(const char* layerName1, const char* layerName2){
	int b1, c1, h1, w1;
	float *mbox_loc_flattened = get_layer_output(layerName1, b1, c1, h1, w1);
	int b2, c2,h2, w2;
	float *mbox_conf_flattened = get_layer_output(layerName2, b2, c2, h2, w2);
#ifndef SNAP_ORDER_FIX
	Tensor2D<float> mbox_loc(mbox_loc_flattened, num_priors, 4);
	Tensor2D<float> mbox_conf(mbox_conf_flattened, num_priors, 5);

#else
	Tensor2D<float> mbox_loc(mbox_loc_flattened, num_priors, 4);
	Tensor2D<float> mbox_conf(mbox_conf_flattened, num_priors, 5);

#endif
	auto start_time = CURRENT_TIME;
	std::vector<std::vector<float> > decoded_boxes = DecodeBoxesSSD::decode(mbox_loc);
	std::vector<std::vector<float> > final_rois = NMS_SSD::nms(decoded_boxes, mbox_conf);
	auto finish = CURRENT_TIME;
    LG<<"VZ Debug: Time taken for decode + nms: "<< finish - start_time <<" ms";
	return final_rois;
}

std::vector<std::vector<float> > DetectorNet::get_output_customlayer(const char* layerName){
	int b, c, h, w;
	float *layer_output_flattened;
	std::vector<int> output_shape;
	if(std::string(layerName) == std::string("output")){
		layer_output_flattened = get_layer_output_shape(layerName, output_shape); //get_layer_output_internal(layerName, output_shape); //
		//layer_output_flattened = get_snap_raw_buffer(output_shape);
		// LOGV("VZ Debug: get_output_customlayer Shape size: [%d] ", output_shape.size());
	}else if(std::string(layerName) == std::string("Identity")){
		layer_output_flattened = get_layer_output_shape(layerName, output_shape);
		// layer_output_flattened = get_snap_raw_buffer(output_shape);
		// LOGV("VZ Debug: get_output_customlayer Shape size: [%d] ", output_shape.size());
	}
	LOGV("VZ Debug: get_output_customlayer V31 layer_name=[%s] c=[%d] h=[%d] w=[%d]", layerName, output_shape[0], output_shape[1], output_shape[2]);
	//this sets the values in chw format
	c = output_shape[0]; h = output_shape[1]; w = output_shape[2];
#ifdef DUMP_OUTPUT
	bool dumpresult = detector_dump_outputlayer(layer_output_flattened,1,c,h,w);
	if(dumpresult){
		#ifdef DUMPBOX
			LOGV("VZ Debug: get_output_customlayer : dump success");
		#endif

	}else{
		#ifdef DUMPBOX
			LOGV("VZ Debug: get_output_customlayer : dump failed");
		#endif
	}
#endif

#ifndef SNAP_ORDER_FIX
	Tensor2D<float> output_tensor(layer_output_flattened, h, w);
#else
	Tensor2D<float> output_tensor(layer_output_flattened, h, w);
#endif
	

	if(layerName == std::string("output") || layerName == std::string("Identity")){
		// auto start_time = CURRENT_TIME;
		// LOGV("VZ Debug: score_threshold[%f] : nms_threshold = [%f]", m_decoder->score_threshold, m_decoder->nms_threshold);
		final_boxes.clear();
		// LOGV("VZ Debug: Not Skipping m_decoder final_boxes ");
		final_boxes = m_decoder->decode(output_tensor, m_decoder->score_threshold, m_decoder->nms_threshold);

		// LOGV("VZ Debug: Passing dummy final_boxes ");
		// final_boxes.emplace_back(6);
		// final_boxes[0][0] = 0.0;
		// final_boxes[0][1] = 0.191948;
		// final_boxes[0][2] = 0.804989;
		// final_boxes[0][3] = 0.755359;
		// final_boxes[0][4] = 0.905581;
		// final_boxes[0][5] = 5.0;
		
		// auto finish = CURRENT_TIME;
    	// LG<<"VZ Debug: Time taken for decode + nms V3: "<< finish - start_time <<" ms";
	}
	else {
		//Should not reach here
		#ifdef DUMPBOX
			LOGV("VZ Debug: Error ! Missing Decoder!");
		#endif
	}
	//LOGV("VZ Debug:  get_output_customlayer: final_rois size=[%d]", final_boxes.size());
	return final_boxes;
}