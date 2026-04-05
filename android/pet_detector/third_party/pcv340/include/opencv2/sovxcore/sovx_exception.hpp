/*******************************************************************************
 *
 *  (C) Mobile Communication Business, Samsung Electronics 2016
 *        Next Generation Camera Development Group
 *
 *   Scope of work: Try/Catch implementation to handle sovx exception.
 *
 *   2016.11.3 Initial implementation
 *
 *   Authors:
 *     - Jeon Cheol-Yong <cy77.jeon@samsung.com>
 *
 *******************************************************************************/

#ifndef SOVX_EXCEPTION_H
#define SOVX_EXCEPTION_H
#include "opencv2/core/base.hpp"
#include "opencv2/sovxcore.hpp"
#include <android/log.h>

#define SOVX_Assert( expr ) if(!!(expr)) ; else throw cv::Exception((cv::Error::SovxNotSupported), (#expr), __func__, __FILE__, __LINE__)
#define SOVX_Error( expr, args )						\
	if(!!(expr)) ;										\
	else												\
	{													\
		args->releaseObj();								\
		throw cv::Exception((cv::Error::SovxNotSupported), (#expr), __func__, __FILE__, __LINE__); \
	}

#define SOVX_TRY();					\
	try{						\
		cv::sovx::SOVXPLS::getInstance()->gotErr = false;\

#ifdef PCV_PROFILING
#define SOVX_CATCH(func);				\
}							\
catch(cv::Exception& e)					\
{							\
	/*						\
	 * -227: SovxNotSupported			\
	 * -228: SovxApiCallError			\
	 */						\
	\
	if(e.code <= cv::Error::SovxNotSupported){	\
		cv::sovx::SOVXPLS::getInstance()->gotErr = true;	\
		printf("Dsp Exception:%s\n", e.what());	\
		__android_log_print(ANDROID_LOG_INFO,	\
				"PCV_SOVX_EXCEPTION",		\
				"SOVX Exception:%s", e.what());	\
		return (func);				\
	}						\
	else{						\
		CV_Error(e.code, e.what());		\
	}						\
}
#else
#define SOVX_CATCH(func);				\
}							\
catch(cv::Exception& e)					\
{							\
	/*						\
	 * -227: SovxNotSupported			\
	 * -228: SovxApiCallError			\
	 */						\
	\
	if(e.code <= cv::Error::SovxNotSupported){	\
		cv::sovx::SOVXPLS::getInstance()->gotErr = true;	\
		__android_log_print(ANDROID_LOG_INFO,	\
				"PCV_SOVX_EXCEPTION",		\
				"SOVX Exception:%s", e.what());	\
		return (func);				\
	}						\
	else{						\
		CV_Error(e.code, e.what());		\
	}						\
}
#endif
#endif
