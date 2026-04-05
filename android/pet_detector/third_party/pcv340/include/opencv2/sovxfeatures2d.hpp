/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __OPENCV_SOVXFEATURES_2D_HPP__
#define __OPENCV_SOVXFEATURES_2D_HPP__

#include "opencv2/core.hpp"
#include "opencv2/flann/miniflann.hpp"

/**
  @defgroup features2d 2D Features Framework
  @{
    @defgroup features2d_main Feature Detection and Description
    @defgroup features2d_match Descriptor Matchers

Matchers of keypoint descriptors in OpenCV have wrappers with a common interface that enables you to
easily switch between different algorithms solving the same problem. This section is devoted to
matching descriptors that are represented as vectors in a multidimensional space. All objects that
implement vector descriptor matchers inherit the DescriptorMatcher interface.

@note
   -   An example explaining keypoint matching can be found at
        opencv_source_code/samples/cpp/descriptor_extractor_matcher.cpp
    -   An example on descriptor matching evaluation can be found at
        opencv_source_code/samples/cpp/detector_descriptor_matcher_evaluation.cpp
    -   An example on one to many image matching can be found at
        opencv_source_code/samples/cpp/matching_to_many_images.cpp

    @defgroup features2d_draw Drawing Function of Keypoints and Matches
    @defgroup features2d_category Object Categorization

This section describes approaches based on local 2D features and used to categorize objects.

@note
   -   A complete Bag-Of-Words sample can be found at
        opencv_source_code/samples/cpp/bagofwords_classification.cpp
    -   (Python) An example using the features2D framework to perform object categorization can be
        found at opencv_source_code/samples/python2/find_obj.py

  @}
 */

namespace cv
{
	namespace sovx
	{
		class CV_EXPORTS_W DescriptorMatcher : public Algorithm
		{
			public:
			virtual ~DescriptorMatcher();

			/** @brief Adds descriptors to train a CPU(trainDescCollectionis) or GPU(utrainDescCollectionis) descriptor
			  collection.

			  If the collection is not empty, the new descriptors are added to existing train descriptors.

			  @param descriptors Descriptors to add. Each descriptors[i] is a set of descriptors from the same
			  train image.
			 */
			CV_WRAP virtual void add( InputArrayOfArrays descriptors );

			/** @brief Returns a constant link to the train descriptor collection trainDescCollection .
			 */
			CV_WRAP const std::vector<Mat>& getTrainDescriptors() const;

			/** @brief Clears the train descriptor collections.
			 */
			CV_WRAP virtual void clear();

			/** @brief Returns true if there are no train descriptors in the both collections.
			 */
			CV_WRAP virtual bool empty() const;

			/** @brief Returns true if the descriptor matcher supports masking permissible matches.
				 */
			CV_WRAP virtual bool isMaskSupported() const = 0;

			/** @brief Trains a descriptor matcher

			  Trains a descriptor matcher (for example, the flann index). In all methods to match, the method
			  train() is run every time before matching. Some descriptor matchers (for example, BruteForceMatcher)
			  have an empty implementation of this method. Other matchers really train their inner structures (for
			  example, FlannBasedMatcher trains flann::Index ).
			 */
			CV_WRAP virtual void train();

			/** @brief Finds the best match for each descriptor from a query set.

			  @param queryDescriptors Query set of descriptors. Descriptors.cols should be 32.
			  @param trainDescriptors Train set of descriptors. Descriptors.cols should be 32. 
			  This set is not added to the train descriptors collection stored in the class object.
			  @param matches Matches. If a query descriptor is masked out in mask , no match is added for this
			  descriptor. So, matches size may be smaller than the query descriptors count.
			  @param mask Mask specifying permissible matches between an input query and train matrices of
			  descriptors.

			  In the first variant of this method, the train descriptors are passed as an input argument. In the
			  second variant of the method, train descriptors collection that was set by DescriptorMatcher::add is
			  used. Optional mask (or masks) can be passed to specify which query and training descriptors can be
			  matched. Namely, queryDescriptors[i] can be matched with trainDescriptors[j] only if
			  mask.at\<uchar\>(i,j) is non-zero.
			 */
			CV_WRAP void match( InputArray queryDescriptors, InputArray trainDescriptors,
					CV_OUT std::vector<DMatch>& matches, InputArray mask=noArray() ) const;

			/** @brief Finds the k best matches for each descriptor from a query set.
			  @param queryDescriptors Query set of descriptors.
			  @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
			  collection stored in the class object.
			  @param mask Mask specifying permissible matches between an input query and train matrices of
			  descriptors.
			  @param matches Matches. Each matches[i] is k or less matches for the same query descriptor.
			  @param k Count of best matches found per each query descriptor or less if a query descriptor has
			  less than k possible matches in total.
			  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
			  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
			  the matches vector does not contain matches for fully masked-out query descriptors.

			  These extended variants of DescriptorMatcher::match methods find several best matches for each query
			  descriptor. The matches are returned in the distance increasing order. See DescriptorMatcher::match
			  for the details about query and train descriptors.
			 */
			CV_WRAP void knnMatch( InputArray queryDescriptors, InputArray trainDescriptors,
					CV_OUT std::vector<std::vector<DMatch> >& matches, int k,
					InputArray mask=noArray(), bool compactResult=false ) const;

			/** @brief For each query descriptor, finds the training descriptors not farther than the specified distance.

			  @param queryDescriptors Query set of descriptors.
			  @param trainDescriptors Train set of descriptors. This set is not added to the train descriptors
			  collection stored in the class object.
			  @param matches Found matches.
			  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
			  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
			  the matches vector does not contain matches for fully masked-out query descriptors.
			  @param maxDistance Threshold for the distance between matched descriptors. Distance means here
			  metric distance (e.g. Hamming distance), not the distance between coordinates (which is measured
			  in Pixels)!
			  @param mask Mask specifying permissible matches between an input query and train matrices of
			  descriptors.

			  For each query descriptor, the methods find such training descriptors that the distance between the
			  query descriptor and the training descriptor is equal or smaller than maxDistance. Found matches are
			  returned in the distance increasing order.
			 */
			void radiusMatch( InputArray queryDescriptors, InputArray trainDescriptors,
					std::vector<std::vector<DMatch> >& matches, float maxDistance,
					InputArray mask=noArray(), bool compactResult=false ) const;

			/** @overload
			  @param queryDescriptors Query set of descriptors.
			  @param matches Matches. If a query descriptor is masked out in mask , no match is added for this
			  descriptor. So, matches size may be smaller than the query descriptors count.
			  @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
			  descriptors and stored train descriptors from the i-th image trainDescCollection[i].
			 */
			CV_WRAP void match( InputArray queryDescriptors, CV_OUT std::vector<DMatch>& matches,
						InputArrayOfArrays masks=noArray() );
			/** @overload
			  @param queryDescriptors Query set of descriptors.
			  @param matches Matches. Each matches[i] is k or less matches for the same query descriptor.
			  @param k Count of best matches found per each query descriptor or less if a query descriptor has
			  less than k possible matches in total.
			  @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
			  descriptors and stored train descriptors from the i-th image trainDescCollection[i].
			  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
			  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
			  the matches vector does not contain matches for fully masked-out query descriptors.
			 */
			CV_WRAP void knnMatch( InputArray queryDescriptors, CV_OUT std::vector<std::vector<DMatch> >& matches, int k,
					InputArrayOfArrays masks=noArray(), bool compactResult=false );
			/** @overload
			  @param queryDescriptors Query set of descriptors.
			  @param matches Found matches.
			  @param maxDistance Threshold for the distance between matched descriptors. Distance means here
			  metric distance (e.g. Hamming distance), not the distance between coordinates (which is measured
			  in Pixels)!
			  @param masks Set of masks. Each masks[i] specifies permissible matches between the input query
			  descriptors and stored train descriptors from the i-th image trainDescCollection[i].
			  @param compactResult Parameter used when the mask (or masks) is not empty. If compactResult is
			  false, the matches vector has the same size as queryDescriptors rows. If compactResult is true,
			  the matches vector does not contain matches for fully masked-out query descriptors.
			 */
			void radiusMatch( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, float maxDistance,
					InputArrayOfArrays masks=noArray(), bool compactResult=false );

			// Reads matcher object from a file node
			virtual void read( const FileNode& );
			// Writes matcher object to a file storage
			virtual void write( FileStorage& ) const;

			/** @brief Clones the matcher.

			  @param emptyTrainData If emptyTrainData is false, the method creates a deep copy of the object,
			  that is, copies both parameters and train data. If emptyTrainData is true, the method creates an
			  object copy with the current parameters but with empty train data.
			 */
			virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const = 0;

			/** @brief Creates a descriptor matcher of a given type with the default parameters (using default
			  constructor).

			  @param descriptorMatcherType Descriptor matcher type. Now the following matcher types are

			supported:
			-   `BruteForce` (it uses L2 )
			-   `BruteForce-L1`
			-   `BruteForce-Hamming`
			-   `BruteForce-Hamming(2)`
			-   `FlannBased`
			 */
			CV_WRAP static Ptr<DescriptorMatcher> create( const String& descriptorMatcherType );
			protected:
			/**
			 * Class to work with descriptors from several images as with one merged matrix.
			 * It is used e.g. in FlannBasedMatcher.
			 */
			class CV_EXPORTS DescriptorCollection
			{
				public:
				DescriptorCollection();
				DescriptorCollection( const DescriptorCollection& collection );
				virtual ~DescriptorCollection();

				// Vector of matrices "descriptors" will be merged to one matrix "mergedDescriptors" here.
				void set( const std::vector<Mat>& descriptors );
				virtual void clear();

				const Mat& getDescriptors() const;
				const Mat getDescriptor( int imgIdx, int localDescIdx ) const;
				const Mat getDescriptor( int globalDescIdx ) const;
				void getLocalIdx( int globalDescIdx, int& imgIdx, int& localDescIdx ) const;

				int size() const;

				protected:
				Mat mergedDescriptors;
				std::vector<int> startIdxs;
			};

			//! In fact the matching is implemented only by the following two methods. These methods suppose
			//! that the class object has been trained already. Public match methods call these methods
			//! after calling train().
			virtual void knnMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, int k,
					InputArrayOfArrays masks=noArray(), bool compactResult=false ) = 0;
			virtual void radiusMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, float maxDistance,
					InputArrayOfArrays masks=noArray(), bool compactResult=false ) = 0;

			static bool isPossibleMatch( InputArray mask, int queryIdx, int trainIdx );
			static bool isMaskedOut( InputArrayOfArrays masks, int queryIdx );

			static Mat clone_op( Mat m ) { return m.clone(); }
			void checkMasks( InputArrayOfArrays masks, int queryDescriptorsCount ) const;

			//! Collection of descriptors from train images.
			std::vector<Mat> trainDescCollection;
			std::vector<UMat> utrainDescCollection;
		};

		/** @brief Brute-force descriptor matcher.

		  For each descriptor in the first set, this matcher finds the closest descriptor in the second set
		  by trying each one. This descriptor matcher supports masking permissible matches of descriptor
		  sets.
		 */
		class CV_EXPORTS_W BFMatcher : public DescriptorMatcher
		{
			public:
			/** @brief Brute-force matcher constructor.

			  @param normType One of NORM_L1, NORM_L2, NORM_HAMMING, NORM_HAMMING2. L1 and L2 norms are
			  preferable choices for SIFT and SURF descriptors, NORM_HAMMING should be used with ORB, BRISK and
			  BRIEF, NORM_HAMMING2 should be used with ORB when WTA_K==3 or 4 (see ORB::ORB constructor
			  description).
			  @param crossCheck If it is false, this is will be default BFMatcher behaviour when it finds the k
			  nearest neighbors for each query descriptor. If crossCheck==true, then the knnMatch() method with
			  k=1 will only return pairs (i,j) such that for i-th query descriptor the j-th descriptor in the
			  matcher's collection is the nearest and vice versa, i.e. the BFMatcher will only return consistent
			  pairs. Such technique usually produces best results with minimal number of outliers when there are
			  enough matches. This is alternative to the ratio test, used by D. Lowe in SIFT paper.
			 */
			CV_WRAP BFMatcher( int normType=NORM_HAMMING, bool crossCheck=false );
			virtual ~BFMatcher() {}

			virtual bool isMaskSupported() const { return true; }

			virtual Ptr<DescriptorMatcher> clone( bool emptyTrainData=false ) const;

			protected:
			virtual void knnMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, int k,
					InputArrayOfArrays masks=noArray(), bool compactResult=false );
			virtual void radiusMatchImpl( InputArray queryDescriptors, std::vector<std::vector<DMatch> >& matches, float maxDistance,
					InputArrayOfArrays masks=noArray(), bool compactResult=false );

			int normType;
			bool crossCheck;
		};
	} /* namespace sovx */

//! @} features2d

} /* namespace cv */

#endif
