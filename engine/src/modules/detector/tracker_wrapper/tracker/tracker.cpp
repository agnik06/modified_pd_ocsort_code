//#include<tracker.hpp>

#include<stdio.h>
// #include<cv.h>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/video/tracking.hpp>
// #include<highgui.h>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/calib3d/calib3d.hpp>
#include<math.h>
#include<iostream>
#include<string>
#include "logger/logger.hpp"


class Tracker{
	private:
	cv::Mat object,img,next_img,homography;
	int image_width,image_height,box_width,box_height;
	float scale;
	std::vector<cv::Point2f> base_box,tracked_box,left_box,right_box,top_box,bottom_box,
							 base_features,
							 tracked_features,
							 useful_base_features,
							 useful_tracked_features;
							 

	std::vector<std::vector<cv::Point2f> > appendage_boxes;

	bool has_box;
	std::string object_tag;
	int no_of_features,min_features,missed_frame_count,frame_count,max_frame_to_observe;
	std::vector<u_char> status;
	std::vector<float> error;
	std::vector<std::pair<float,int> > feature_list;



	void set_good_features(){
		cv::goodFeaturesToTrack(object,base_features,no_of_features,0.1,0.1);
		for(int i=0;i<base_features.size();i++){
			base_features[i].x=std::min(base_features[i].x+base_box[0].x,(float)img.cols);
			base_features[i].y=std::min(base_features[i].y+base_box[0].y,(float)img.rows);
		}
	}

	void set_appendage_boxes(){
		//left_box
		left_box.clear();
		right_box.clear();
		top_box.clear();
		bottom_box.clear();
		left_box={cv::Point2f(base_box[0].x-0.5*box_width),cv::Point2f(base_box[0].y),
							cv::Point2f(base_box[0].x),cv::Point2f(base_box[1].y)};
		
		left_box={cv::Point2f(base_box[0].x-0.5*box_width),cv::Point2f(base_box[0].y),
							cv::Point2f(base_box[0].x),cv::Point2f(base_box[1].y)};

		top_box={cv::Point2f(base_box[0].x),cv::Point2f(base_box[0].y-0.5*box_height),
							cv::Point2f(base_box[1].x),cv::Point2f(base_box[1].y)};
		
		left_box={cv::Point2f(base_box[0].x-0.5*box_width),cv::Point2f(base_box[0].y),
							cv::Point2f(base_box[0].x),cv::Point2f(base_box[1].y)};

		left_box={cv::Point2f(base_box[0].x-0.5*box_width),cv::Point2f(base_box[0].y),
							cv::Point2f(base_box[0].x),cv::Point2f(base_box[1].y)};
	}

	int check_bound(std::vector<cv::Point2f> &points){
		for(int i=0;i<points.size();i++){
			if((points[i].x>=img.cols||points[i].y>=img.rows||points[i].x<0||points[i].y<0)){
				return 0;
			}
		}
		return 1;
	}

	bool check_box_scale(std::vector<cv::Point2f> &points){
		if(points[0].y>points[1].y){
			return 0;
		}
		if(points[0].x>points[1].x){
			return 0;
		}
		// float new_scale=points[1].y-points[0].y;
		// new_scale/=points[1].x-points[0].x;
		// if(new_scale<0){
		// 	new_scale=1/new_scale;
		// }
		// if((new_scale-scale)>(scale)){
		// 	return 0;
		// }
		return 1;
	}

	bool check_for_smaller_box(int box_width,int box_height){
		if(box_width<0.2*image_width){
			return 1;
		}
		if(box_height<0.2*image_height){
			return 1;
		}
		return 0;
	}

	bool process(){
		// std::cout<<"in_process";
		// cv::imshow("test",next_img);
		// cv::imshow("test1",img);
		int box_width=base_box[1].x-base_box[0].x,box_height=base_box[1].y-base_box[0].y;
		if(check_for_smaller_box(box_width,box_height)){
			return 0;
		}
		cv::Size search_window=cv::Size((base_box[1].x-base_box[0].x)/2,(base_box[1].y-base_box[0].y)/2);
		std::vector<cv::Mat> img_pry,next_img_pry;
		int pryd_size;
		pryd_size=cv::buildOpticalFlowPyramid(img,img_pry,search_window,16);
		cv::buildOpticalFlowPyramid(next_img,next_img_pry,search_window,16);
		if(pryd_size<=1){
			return 0;
		}
		#ifdef DUMPBOX
			LOGV("CD Debug:Pyramid_size=%d",pryd_size);
			LOGV("CD Debug:80");
		#endif
		cv::calcOpticalFlowPyrLK(	img_pry,
									next_img_pry,
									base_features,
									tracked_features,
									status,
									error,
									search_window,
									5,
									cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS,30,0.001),
									0,
									0.001);
		#ifdef DUMPBOX
			LOGV("CD Debug:92");
		#endif
		int flag=1;
		std::cout<<status.size();
		feature_list.clear();
		for(int i=0;i<status.size();i++){
			
			
			// if(status[i]=='\0')
			// {
			// 	// std::cout<<error[i]<<"\t";
			// 	std::cout<<"object_not_found";
			// 	flag=0;
			// 	break;
			// }
			if(status[i]!='\0'){
				feature_list.push_back(std::make_pair(error[i],i));
			}
		}
		#ifdef DUMPBOX
			LOGV("CD Debug:110");
		#endif
		if(!check_bound(base_features)){
			return 0;
		}
		#ifdef DUMPBOX
			LOGV("CD Debug:114");
		#endif
		if(!check_bound(tracked_features)){
			return 0;
		}
		#ifdef DUMPBOX
			LOGV("CD Debug:118");
		#endif
		if(feature_list.size()>=min_features){
			sort(feature_list.begin(),feature_list.end());
			useful_tracked_features.clear();
			useful_base_features.clear();
			float min=feature_list[0].first;
			int i=0;
			for(;i<feature_list.size();i++){
				if(feature_list[i].first>2*min){
					break;
				}
				useful_base_features.push_back(base_features[feature_list[i].second]);
				useful_tracked_features.push_back(tracked_features[feature_list[i].second]);
			}
			if(i<min_features){
				flag=0;
			}
		}
		else{
			return 0;
		}
		// std::cout<<std::endl;
		if(flag==0){
			return 0;
		}
		#ifdef DUMPBOX	
			LOGV("CD Debug:143");
		#endif
		homography=cv::findHomography(useful_base_features,useful_tracked_features,cv::RANSAC);
		cv::perspectiveTransform(base_box,tracked_box,homography);
		#ifdef DUMPBOX
			LOGV("CD Debug:146");
		#endif
		img=next_img.clone();
		int check=check_bound(tracked_box);
		    
		if(check){
			base_box=tracked_box;
		
		if(!check_box_scale(tracked_box)){
			return 0;
		}
		#ifdef DUMPBOX
			LOGV("CD Debug:156");
		#endif
		//std::cout<<"73\n";
			object=img(cv::Rect(base_box[0].x,base_box[0].y,base_box[1].x-base_box[0].x,base_box[1].y-base_box[0].y));
		#ifdef DUMPBOX
			LOGV("CD Debug:159");
		#endif
		// std::cout<<"75\n";
			base_features=tracked_features;
		// std::cout<<"78\n";
		return 1;
		}
		else{
			return 0;
		}
		
	}

	public:

	bool box_available(){
		return has_box;
	}

	std::vector<cv::Point2f> get_tracked_boxes(cv::Mat _img){
		if(!has_box){
			return {};
		}
		cv::cvtColor(_img,next_img,cv::COLOR_RGB2GRAY);
		int p=process();
		if(p){
			has_box=1;
			frame_count++;
			if(frame_count>max_frame_to_observe){
				has_box=0;
				frame_count=0;
				return {};
			}
			missed_frame_count=0;
			return base_box;
		}
		else{
			missed_frame_count++;
			frame_count=0;
			if(missed_frame_count>=30){
				has_box=0;
				return {};
			}
			
		}
	}

	int set_tracker(cv::Mat img_rgb,cv::Point2f top_left,cv::Point2f bottom_right,int _no_of_features=20,int _min_features=4,std::string tag="none",int _max_frame_to_observe=60){
		cv::cvtColor(img_rgb,img,cv::COLOR_RGB2GRAY);
		image_width=img_rgb.cols;
		image_height=img_rgb.rows;
		no_of_features=_no_of_features;
		min_features=_min_features;
		base_box={top_left,bottom_right};
		scale=(((bottom_right.x-top_left.x)/bottom_right.y-top_left.y));
		if(scale<1){
			scale=1/scale;
		}
		box_width=base_box[1].x-base_box[0].x;
		box_height=base_box[1].y-base_box[0].y;
		int check=check_bound(base_box);
		if(!check){
			return 0;
		}
		object=img(cv::Rect(top_left.x,top_left.y,bottom_right.x-top_left.x,bottom_right.y-top_left.y));
		set_good_features();
		has_box=1;
		missed_frame_count=0;
		frame_count=0;
		max_frame_to_observe=_max_frame_to_observe;
		return 1;
	}


};




// int main(){
// 	int file=2;
// 	cv::Mat img_rgb=cv::imread("../img/0001.jpg"),img_hsv,car,img;/////////
// 	int no_of_features=25;
// 	std::vector<cv::Point2f> box={cv::Point2f(165,123),cv::Point2f(185,141)};
// 	Tracker tracker;
// 	tracker.set_tracker(img_rgb,box[0],box[1],5);

// 	cv::imshow("imag2s",img_rgb);
	
// 	while(file<1000){

// 		std::string str=std::to_string(file)+".jpg";
// 		if(file<10){
// 			str="000"+str;
// 		}
// 		else if(file<100){
// 			str="00"+str;
// 		}
// 		else if(file<1000){
// 			str="0"+str;
// 		}
// 		str="../img/"+str;
// 		// std::cout<<str;
// 		cv::Mat next_img_rgb=cv::imread(str);
// 		box=tracker.get_tracked_boxes(next_img_rgb);
// 		std::cout<<"done";
// 		if(box.size()!=0){
// 			cv::rectangle(next_img_rgb,box[0],box[1],(cv::Scalar(255,0,0)));

// 		}
		
// 		cv::imshow("imag1s",next_img_rgb);

		

// 		cv::waitKey(-1);
// 		file+=3;
// 	}
	
// 	return 0;
// }
