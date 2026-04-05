/**
 * @file   bounding_box_utils.cpp
 * @brief  This file implements different bounding_box utilities
 *
 * This file implements different bounding_box utilities
 */

#include "bounding_box_utils.hpp"
#include "logger/logger.hpp"

// Calculate the Intersection over Union (IoU) between two bounding boxes
double calculateIoU(const vizpet::Rectangle& box1, const vizpet::Rectangle& box2) {
    int x1 = std::max(box1.left, box2.left);
    int y1 = std::max(box1.top, box2.top);
    int x2 = std::min(box1.right , box2.right);
    int y2 = std::min(box1.bottom, box2.bottom);
    if (x2 <= x1 || y2 <= y1) {
        return 0.0;
    }
    int intersectionArea = (x2 - x1) * (y2 - y1);
    // box1.width * box1.height + box2.width *box2.height - intersectionArea
    int totalArea = (box1.right-box1.left+1) * (box1.bottom-box1.top+1) + (box2.right-box2.left+1) * (box2.bottom-box2.top+1) - intersectionArea;
    return static_cast<double>(intersectionArea) / totalArea;
}

// Merge bounding boxes with overlapping IoU greater than the threshold
std::vector<vizpet::BoundingBox> mergeBoundingBoxes(const std::vector<vizpet::BoundingBox>& boxes, double iouThreshold) {
    std::vector<vizpet::BoundingBox> mergedBoxes;
    for (const vizpet::BoundingBox& box : boxes) {
        bool shouldMerge = false;
        for (const vizpet::BoundingBox& mergedBox : mergedBoxes) {
            double iou = calculateIoU(box.location, mergedBox.location);
            if (iou >= iouThreshold) {
                shouldMerge = true;
                break;
            }
        }
        if (!shouldMerge) {
            mergedBoxes.push_back(box);
        }
    }
    return mergedBoxes;
}

// Function to stabilize focus
cv::Mat stabilizeFocus(const cv::Mat& inputImage) {
    cv::Mat blurredImage;
    cv::GaussianBlur(inputImage, blurredImage, cv::Size(0, 0), 3);
    cv::Mat sharpened;
    cv::addWeighted(inputImage, 1.5, blurredImage, -0.5, 0, sharpened);
    return sharpened;
}

// Function to stabilize lighting
cv::Mat stabilizeLighting(const cv::Mat& inputImage) {
    cv::Mat normalizedImage;
    cv::normalize(inputImage, normalizedImage, 0, 255, cv::NORM_MINMAX);
    return normalizedImage;
}

// Calculate the Laplacian variance as a measure of motion blur
double calculateLaplacianVariance(const cv::Mat& frame) {
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    cv::Mat laplacian;
    cv::Laplacian(grayFrame, laplacian, CV_64F);
    cv::Scalar mean, stddev;
    cv::meanStdDev(laplacian, mean, stddev);
    double variance = stddev.val[0] * stddev.val[0];
    return variance;
}

// Getting the smoother box
vizpet::Rectangle update_smother_box(const vizpet::Rectangle& newBox,vizpet::Rectangle& smother_box){
    double alpha = 0.2;
    smother_box.left = alpha * newBox.left + (1 - alpha) * smother_box.left;
    smother_box.top = alpha * newBox.top + (1 - alpha) * smother_box.top;
    smother_box.right = alpha * newBox.right + (1 - alpha) * smother_box.right;
    smother_box.bottom = alpha * newBox.bottom + (1 - alpha) * smother_box.bottom;
    return smother_box;
}

vizpet::Rectangle update_weighted_smoother_box(const vizpet::Rectangle& newBox,vizpet::Rectangle& smoothedBox){
    double weight = calculateWeight(newBox,smoothedBox);
    #ifdef DUMPBOX
        LOGV("update_weighted_smoother_box : [%lf]", weight);
    #endif
    // Exponential moving average update
    smoothedBox.left = static_cast<int>((1 - weight) * newBox.left + weight * smoothedBox.left);
    smoothedBox.top = static_cast<int>((1 - weight) * newBox.top + weight * smoothedBox.top);
    smoothedBox.right = static_cast<int>((1 - weight) * newBox.right + weight * smoothedBox.right);
    smoothedBox.bottom = static_cast<int>((1 - weight) * newBox.bottom + weight * smoothedBox.bottom);
    return smoothedBox;
}

 double calculateWeight(const vizpet::Rectangle& box, vizpet::Rectangle& smother_box) {
    // Calculate weight based on box size or any other relevant factor
    // You can adjust the calculation based on your needs
    double weight = 0.2;  // Default weight
    // For example, you can assign a higher weight to larger boxes
    int box_width = box.right-box.left+1;
    int box_height = box.bottom-box.top+1;
    int smoothed_box_width = smother_box.right-smother_box.left+1;
    int smoothed_box_height = smother_box.bottom-smother_box.top+1;
    weight += 0.03 * (box_width * box_height) / (smoothed_box_width * smoothed_box_height);
    return std::min(1.0, std::max(0.0, weight));  // Clamp between 0 and 1
}

double calculateDistance(const vizpet::BoundingBox& box1, const vizpet::BoundingBox& box2) {
    int box1_width = box1.location.right-box1.location.left+1;
    int box1_height = box1.location.bottom-box1.location.top+1;

    int box2_width = box2.location.right-box2.location.left+1;
    int box2_height = box2.location.bottom-box2.location.top+1;

    int centerX1 = box1.location.left + box1_width / 2;
    int centerY1 = box1.location.top + box1_height / 2;
    int centerX2 = box2.location.left + box2_width / 2;
    int centerY2 = box2.location.top + box2_height / 2;
    double distanceX = std::abs(centerX1 - centerX2);
    double distanceY = std::abs(centerY1 - centerY2);
    return std::sqrt(distanceX * distanceX + distanceY * distanceY);
}

vizpet::BoundingBox mergeBoxes(const std::vector<vizpet::BoundingBox>& boxes) {
    int minX = boxes[0].location.left;
    int minY = boxes[0].location.top;
    int maxX = boxes[0].location.right;
    int maxY = boxes[0].location.bottom;
    for (const vizpet::BoundingBox& box : boxes) {
        minX = std::min(minX, box.location.left);
        minY = std::min(minY, box.location.top);
        maxX = std::max(maxX, box.location.right);
        maxY = std::max(maxY, box.location.bottom);
    }
    return vizpet::BoundingBox{{minX, minY, maxX, maxY},boxes[0].image_size,boxes[0].tag,boxes[0].score,boxes[0].tag_id,boxes[0].trackid};
}


std::vector<vizpet::BoundingBox> adptiveMergeBoundingBoxes(const std::vector<vizpet::BoundingBox>& boxes){
     double maxMergeDistanceMultiple = 2.0; // You can adjust this multiple based on your needs
    std::vector<vizpet::BoundingBox> mergedBoxes;
    std::vector<bool> mergedFlag(boxes.size(), false);
    for (size_t i = 0; i < boxes.size(); ++i) {
        if (mergedFlag[i]) {
            continue;
        }
        double closestDistance = std::numeric_limits<double>::max();
        size_t closestIdx = i;
        for (size_t j = i + 1; j < boxes.size(); ++j) {
            double distance = calculateDistance(boxes[i], boxes[j]);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestIdx = j;
            }
        }
        if (!mergedFlag[closestIdx] && closestDistance < maxMergeDistanceMultiple * calculateDistance(boxes[i], boxes[closestIdx])) {
            vizpet::BoundingBox mergedBox = mergeBoxes({boxes[i], boxes[closestIdx]});
            mergedBoxes.push_back(mergedBox);
            mergedFlag[i] = true;
            mergedFlag[closestIdx] = true;
        }
    }
    return mergedBoxes;
}

// Define a function to check and remove boxes within boxes
/*std::vector<vizpet::BoundingBox> checkAndRemoveBoxes(std::vector<vizpet::BoundingBox>& boxes) {
  // Create an empty vector to store the indices of the remaining boxes
  std::vector<int> indices;

  // Create an empty vector to store the confidence scores of the boxes
  std::vector<float> scores;

  // Loop through the boxes and extract their scores
  for (const auto& box : boxes) {
    scores.push_back(box.score);
  }

  // Convert the boxes to OpenCV Rect format
  std::vector<cv::Rect> rects;
  for (const auto& box : boxes) {
    rects.push_back(cv::Rect(box.location.left, box.location.top, (box.location.right-box.location.left+1), (box.location.bottom-box.location.top+1)));
  }

  // Apply NMS using OpenCV function
  cv::dnn::NMSBoxes(rects, scores, 0.4, 0.3, indices);

  // Create a new vector to store the remaining boxes
  std::vector<vizpet::BoundingBox> new_boxes;

  // Loop through the indices and copy the corresponding boxes to the new vector
  for (const auto& index : indices) {
    new_boxes.push_back(boxes[index]);
  }

  // Replace the original vector with the new vector
  return new_boxes;
}*/

 //A function to check if a bounding box is inside another one 
bool isInside (vizpet::BoundingBox inner, vizpet::BoundingBox outer) { 
    // Check if the top-left corner of the inner box is inside the outer box 
    if (inner.location.left >= outer.location.left && inner.location.top >= outer.location.top) { 
        // Check if the bottom-right corner of the inner box is inside the outer box
        if (inner.location.right <= outer.location.bottom && inner.location.bottom <= outer.location.bottom){ 
            return true; // The inner box is completely inside the outer box 
         } 
    } 
    return false; // The inner box is not completely inside the outer box 
}

// A function to find and remove nested bounding boxes from a vector 
void removeNestedBoxes (std::vector<vizpet::BoundingBox>& boxes) { 
    // Iterate over the vector from the end to the beginning 
    for (int i = boxes.size () - 1; i >= 0; i--) { 
        // Iterate over the vector again and compare each box with the current one 
        for (int j = 0; j < boxes.size (); j++) { 
            // Skip if i and j are equal 
            if (i == j) continue; 
            // Check if the box at index i is inside the box at index j 
            if (isInside (boxes [i], boxes [j])) { 
                // Remove the box at index i from the vector 
                boxes.erase (boxes.begin () + i); 
                break; // Break out of the inner loop 
            } 
        } 
    } 
}
