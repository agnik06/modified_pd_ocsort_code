#include <iostream>
#include <vector>
#include<algorithm>
#include<math.h>
#include "nms_ssd.hpp"
#include "logger/logger.hpp"

float CONFIDENCE_THRESHOLD = 0.300000011921 ;
float OVERLAP_THRESHOLD = 0.34999999404;
int TOP_BOXES = 100;
int NUM_PRIORS = 8190;

Tensor2D<float> softmax(Tensor2D<float> mbox_conf,int num_classes)
{
	int num_priors = NUM_PRIORS;
	float sum_ex = 0.0;
	for(int i=0;i < num_priors;i++)
	{
		float max = -10000;
		sum_ex = 0.0;
		for(int k = 0; k < num_classes; k++)
		{
	        if (max < mbox_conf(i,k))
	        {
	            max = mbox_conf(i,k);
	        }
	    }

	    for(int k = 0; k < num_classes; k++)
	    {
	   
	     	mbox_conf(i,k) -= max;
	     	mbox_conf(i,k) = exp(mbox_conf(i,k));
	        sum_ex+= mbox_conf(i,k);
	      
	    }

      	for(int k = 0; k < num_classes; k++)
	    {
	     	mbox_conf(i,k) /= sum_ex;
	        
	    }
      }
      return mbox_conf;	
}

std::vector<int> get_classification_index(Tensor2D<float> mbox_conf, int num_classes=5)
{
	int num_priors = NUM_PRIORS;
	std::vector<int> classification_index;
	for(int i=0;i < num_priors;i++)
	{
		float max = 0.0;
		int idx = -1;
		for(int k = 1; k < num_classes; k++)
		{
	        if (max < mbox_conf(i,k))
	        {
	            max = mbox_conf(i,k);
	            idx = k;
	        }
	    }
	    if(max>CONFIDENCE_THRESHOLD)
	    	std::cout<<"max is "<<max<<std::endl;
	    classification_index.push_back(idx);

	}
	return classification_index;
}
std::vector<std::vector<float> > NMS_SSD::nms(std::vector<std::vector<float> > rois,Tensor2D<float> mbox_conf, int num_classes)
{
	std::cout<<"in nms "<<std::endl;
	int num_priors = NUM_PRIORS;
	std::cout<<std::endl;
	std::vector<std::vector<float> > final_rois; // 4 coordinates 1 class and other confidence score.
	if (rois.size() == 0)
	{
		return final_rois;
	}
	mbox_conf = softmax(mbox_conf,num_classes);
	std::vector<int> classification_index = get_classification_index(mbox_conf);
	int label;
	std::vector<std::vector<float> > class_roi_vectors[num_classes];
	for(int i = 0; i < num_priors; i++)
	{
		label = classification_index[i];
		if(label != -1 && (mbox_conf(i,label)>=CONFIDENCE_THRESHOLD))
		{
			std::vector<float> roi = {rois[i][0],rois[i][1],rois[i][2],rois[i][3],mbox_conf(i,label),(float)label};
			class_roi_vectors[label].push_back(roi);
		}
		
	}


	float inter_left, inter_top, inter_right, inter_bottom, overlap;
	std::vector<std::vector<float> > boxes;
	for(int k = 1; k< num_classes;k++)
	{
		
		std::vector<std::pair<float,int> > scores;
		for(int i = 0; i < std::min(TOP_BOXES,(int)class_roi_vectors[k].size()); i++)
		{
			scores.push_back({class_roi_vectors[k][i][4],i});
    	}
    	sort(scores.begin(),scores.end());
    	reverse(scores.begin(),scores.end());
    	for(int i=0;i<scores.size();i++)
    	{
    		boxes.push_back(class_roi_vectors[k][scores[i].second]);
    	}
    	
		while(boxes.size() != 0){
			std::vector<int> to_remove(boxes.size(),0);
			int index = 0;
			float box_a_area = (boxes[index][2] - boxes[index][0]) * (boxes[index][3] - boxes[index][1]);
			//std::cout<<" scores top "<<scores[0].first<<std::endl;
			for(int j = 0; j < (int)boxes.size(); j++)
			{
				if (j != index)
				{
					float box_b_area = (boxes[j][2] - boxes[j][0]) * (boxes[j][3] - boxes[j][1]);
					inter_left = fmax(boxes[j][0], boxes[index][0]);
					inter_top = fmax(boxes[j][1], boxes[index][1]);
					inter_right = fmin(boxes[j][2], boxes[index][2]);
					inter_bottom = fmin(boxes[j][3], boxes[index][3]);
					float intersection = (inter_right - inter_left) * (inter_bottom - inter_top);
					
					if((inter_right >= inter_left) && (inter_bottom >= inter_top))
					{

						overlap = intersection / (box_a_area + box_b_area - intersection);
						if(overlap >= OVERLAP_THRESHOLD)
						{
						    to_remove[j] = 1;
            			}
					}
				}
				else
				{
					to_remove[index] = 1;
        		}
			}

			final_rois.push_back(boxes[index]);
			std::vector<std::vector<float> > new_boxes;
			for(int l=to_remove.size()-1; l>=0; l--)
			{
      			int remove = to_remove[l];
      			if(remove == 0)
					new_boxes.push_back(boxes[l]);
			}
			boxes.clear();
			for(int i=0;i<new_boxes.size();i++)
			{
				boxes.push_back(new_boxes[i]);
			}
			new_boxes.clear();
 		}
	}
	#ifdef DUMPBOX
		LOGV("VZ Debug:  nms: final_rois size=[%lu]", final_rois.size());
	#endif
	//delete class_roi_vectors;
	return final_rois;
}