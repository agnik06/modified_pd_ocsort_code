#include "pet_detector.h"
#include "opencv2/opencv.hpp"
#include <iostream>

#include "modules/detector/mapper/data/roi_data_handler.hpp"

#include <thread>
#include <chrono>

#define TRACKER (1)
int test_data_handler(){
    ROIDataHandler data_handler;
    std::cout<<data_handler.get_label(0)<<"\n";
    std::cout<<data_handler.get_priority(0)<<"\n";
    std::cout<<data_handler.is_valid_id(0)<<"\n";
    return 0;
}

void callback_init(bool status)
{
    std::cout<<"venkat callback_init successful in test.cpp";
    return;
}

int test_object_detector(std::string image_path){
    std::shared_ptr<vizpet::PetDetector> object_detector(vizpet::PetDetector::Create(), vizpet::PetDetector::Destroy);
    // vizpet::PetDetector* object_detector = vizpet::PetDetector::Create();

    //Initialize
    bool init_status = object_detector->initialize(callback_init, vizpet::PetDetector::Mode::SALIENCY, (char*)"models/");
    std::cout<<"Init status: "<<init_status<<"\n";
    if(!init_status){
        std::cerr<<"Initialization of PetDetector failed\n";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //object_detector->inittracking();


    //Execute
    vizpet::PetDetector::trkRes boxinfo;
    cv::Mat image = cv::imread(image_path);
    char* image_buffer = reinterpret_cast<char*> (image.data);
    bool execute_status = object_detector->execute_starttracking(image_buffer, image.cols, image.rows, 0, vizpet::PetDetector::ImageFormat::BGR, -1, -1, boxinfo);
    //bool execute_status = object_detector->execute(image_buffer, image.cols, image.rows, 0, vizpet::PetDetector::ImageFormat::BGR);
    // bool execute_status = object_detector->execute_on_touch(image_buffer, image.cols, image.rows, 0, vizpet::PetDetector::ImageFormat::BGR, 100, 300);
    std::cout<<"Execute with start tracking status: "<<execute_status<<"\n";
    if(!execute_status){
        std::cerr<<"Execution of detection failed\n";
    }

    //process
    int processstatus = true;
    while(processstatus){
        vizpet::PetDetector::trkRes keeptrackingboxinfo;
        //if no more frames to process then set processstatus to false
        //read the next frame
        cv::Mat image = cv::imread(image_path);
        char* image_buffer = reinterpret_cast<char*> (image.data);
        processstatus = object_detector->execute_keeptracking(image_buffer, 0, keeptrackingboxinfo);
        std::cout<<"Execute with keep tracking status: "<<processstatus<<"\n";
        if(!processstatus)
            object_detector->stoptracking();
    }
    object_detector->resettracking();

/*
    //Extract boxes
    int n_boxes = object_detector->get_n_boxes();
    vizpet::PetDetector::BoxInfo box_infos[n_boxes]; 

    bool extraction_staus = object_detector->get_box_info(box_infos, n_boxes);
    std::cout<<"Extraction status: "<<extraction_staus<<"\n";
    if(!extraction_staus){
        std::cerr<<"Extraction of result failed\n";
    }
    else{
        for(int i=0; i<n_boxes; i++){
            std::cout<<"Result: "<<box_infos[i].tag<<" | ("<<box_infos[i].image_width<<","<<box_infos[i].image_height<<")\n";
            std::cout<<"Result: "<<"("<<box_infos[i].left<<","<<box_infos[i].top<<","<<box_infos[i].right<<","<<box_infos[i].bottom<<")\n";            
        }
    }

    //Display result
    if(extraction_staus){
        for(int i=0; i<n_boxes; i++){
            cv::Scalar color = cv::Scalar(0,255,0);
            cv::rectangle(image, {box_infos[i].left, box_infos[i].top}, {box_infos[i].right, box_infos[i].bottom}, color);
        }
        cv::imwrite("test/output.png", image);
        //cv::imshow("Output", image);
        //cv::waitKey(0);
    }
   */ 
    // vizpet::PetDetector::Destroy(object_detector);

    return 0;
}

int main(int argc, char** argv){
    std::string image_path("test/test_image.jpg");
    if(argc > 1) image_path = std::string(argv[1]);

    test_object_detector(image_path);
    // test_data_handler();
    return 0;
}
