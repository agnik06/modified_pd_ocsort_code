/**
 * @file   model_config.hpp
 * @brief  This fine defines the V1 and V2 config
 *
 * V1 refers to Mobiledetedgetpu series of models
 * V2 refers to MobileDetDSP series of models
 * Although V1 and V2 have same classes the thresholds will be
 * different since the NN arch is different across both
 */

#ifndef __MODEL_COFIG__
#define __MODEL_COFIG__

namespace model{
    enum class Version{
	    UNKNOWN = 0,
        V1 = 1,
        V2 = 2,
        V2_MULTI_FRAME = 3
    };
}

#endif //__MODEL_COFIG__