#include "light_object_detector.h"
#include "opencv2/opencv.hpp"
#include <iostream>
#include "logger/logger.hpp"
#include <unistd.h>
#include <cstdlib>
#include <dirent.h>
#include <fstream>

int init_complete = false;
void init_cb(bool init_status){
    init_complete = true;
}

std::vector<std::string> split(std::string str,std::string delimiter){
    int idx;
    int begin=0;

    std::vector<std::string>  str_set;
    while(1){
        idx = str.substr(begin).find(delimiter);
        if(idx == 0){
            begin++;
            continue;
        }
        if(idx==-1){
            break;
        }
        str_set.push_back(str.substr(begin,idx));
        begin = begin + idx+1;
    }
    str_set.push_back(str.substr(begin));
    return str_set;
}

int test_object_detector(std::vector<std::string> image_paths,std::string result_folder){
    std::cout<<image_paths[0];
    std::shared_ptr<vizlod::ObjectDetector> object_detector(vizlod::ObjectDetector::Create(), vizlod::ObjectDetector::Destroy);
    // vizlod::ObjectDetector* object_detector = vizlod::ObjectDetector::Create();

    //Initialize
    bool init_status = object_detector->initialize(init_cb, vizlod::ObjectDetector::Mode::SALIENCY, (char*)"/vendor/etc/saiv/image_understanding/db/");
    std::cout<<"Init status: "<<init_status<<"\n";
    if(!init_status){
        std::cerr<<"Initialization of ObjectDetector failed\n";
    }

    while(!init_complete){

        usleep(200);
    }

    //Execute
    std::ofstream outdata ;
    for (auto &image_path : image_paths){
        std::cout<<image_path<<std::endl;
        cv:: Mat image = cv::imread(image_path);
        std::cout<<"image_loaded";
        std::vector<std::string> name_split = split(image_path,"/");
        std::cout<<"......................dump_file_name:"<<*(name_split.end()-1)<<std::endl;
        std::cout<<result_folder+"/"+(*(name_split.end()-1)).substr(0,(*(name_split.end()-1)).find("."))+".txt"<<std::endl;
        outdata.open(result_folder+"/"+(*(name_split.end()-1)).substr(0,(*(name_split.end()-1)).find("."))+".txt");
        char* image_buffer = reinterpret_cast<char*> (image.data);
        bool execute_status = object_detector->execute(image_buffer, image.cols, image.rows, 0, vizlod::ObjectDetector::ImageFormat::BGR);
    // bool execute_status = object_detector->execute_on_touch(image_buffer, image.cols, image.rows, 0, vizlod::ObjectDetector::ImageFormat::BGR, 100, 300);
        std::cout<<"Execute status: "<<execute_status<<"\n";
        if(!execute_status){
            std::cerr<<"Execution failed\n";
        }

    //Extract boxes
        int n_boxes = object_detector->get_n_boxes();
        vizlod::ObjectDetector::BoxInfo box_infos[n_boxes]; 

        bool extraction_staus = object_detector->get_box_info(box_infos, n_boxes);
        std::cout<<"Extraction status: "<<extraction_staus<<"\n";
        if(!extraction_staus){
            std::cerr<<"Extraction of result failed\n";
        }
        else{
            for(int i=0; i<n_boxes; i++){
                std::cout<<"Result: "<<box_infos[i].tag<<" | ("<<box_infos[i].image_width<<","<<box_infos[i].image_height<<")\n";
                std::cout<<"Result: "<<"("<<box_infos[i].left<<","<<box_infos[i].top<<","<<box_infos[i].right<<","<<box_infos[i].bottom<<")\n";            
                outdata<<int(box_infos[i].tag_class)<<" "<<box_infos[i].score<<" "<<box_infos[i].left<<" "<<box_infos[i].top<<" "<<box_infos[i].right<<" "<<box_infos[i].bottom<<"\n";
            }
        }
        outdata.close();
    }
    
    return 0;
}

int main(int argc, char** argv){

    // if(argc > 1) image_path = std::string(argv[1]);
    std::string image_folder ="/sdcard/LOD_Test_Set";
    std:: string result_folder = "/sdcard/result/LOD_Test_Set";
    struct dirent *entry;
    DIR *dir = opendir(image_folder.c_str());
    std::vector<std::string> image_paths;
    while ((entry = readdir(dir)) != NULL){
        if(entry->d_name == std::string("."))
        continue;
       if(entry->d_name == std::string(".."))
        continue;
    
        image_paths.push_back(image_folder+std::string("/")+entry->d_name);
    }

        init_complete = false;
        test_object_detector(image_paths,result_folder);
    
    // test_data_handler();
    return 0;
}
