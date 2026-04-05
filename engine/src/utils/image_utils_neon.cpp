#ifdef ANDROID_ARM_NEON
#include "image_utils.hpp"
#include "logger/logger.hpp"
#include "arm_neon.h"

// using namespace vizpet_arm_neon;

bool vizpet_arm_neon::resize_nv21_buffer(char* yuv, char* yuv_resized, const int image_width, const int image_height, const int target_width, const int target_height){
    float step_w = float(image_width) / target_width;
    float step_h = float(image_height) / target_height;

    std::vector<int> uv(target_width * target_height * 0.5);

    for(int i=0; i<target_height; i++){
        for(int j=0; j<target_width; j++){
            int i_org = i * step_h;
            int j_org = j * step_w;
            // Y
            yuv_resized[i * target_width + j] = 
                yuv[i_org * image_width + j_org];
            // // u
            // yuv_resized[(target_height + i/2) * target_width + (j - j%2)] += 
            //     yuv[(image_height + i_org/2) * image_width + (j_org - j_org%2)];
            // // v
            // yuv_resized[(target_height + i/2) * target_width + (j - j%2 + 1)] += 
            //     yuv[(image_height + i_org/2) * image_width + (j_org - j_org%2 + 1)];

            //uv
            uv[(i/2) * target_width + (j - j%2)] += 
                yuv[(image_height + i_org/2) * image_width + (j_org - j_org%2)];
            // v
            uv[(i/2) * target_width + (j - j%2 + 1)] += 
                yuv[(image_height + i_org/2) * image_width + (j_org - j_org%2 + 1)];
        }
    }

    for(int i=0; i<target_height; i++){
        for(int j=0; j<target_width; j++){
            int idx = (target_height + i/2) * target_width + (j - j%2);
            int idx_uv = idx - target_height * target_width;
            yuv_resized[idx] = uv[idx_uv]/4;
            yuv_resized[idx+1] = uv[idx_uv+1]/4;
            // yuv_resized[(target_height + i/2) * target_width + (j - j%2)] /= 4;
            // yuv_resized[(target_height + i/2) * target_width + (j - j%2 + 1)] /= 4; 
        }
    }
    return true;
}

static inline void vizpet_arm_neon::ics_yuv2rgb_bt709_neon_8I(uint8x8_t y,  uint8x8_t cb,   uint8x8_t cr,   uint8x8_t *r, uint8x8_t *g, uint8x8_t *b){
    /*
    R'= Y' + 0.0000*U + 1.5748*V
    G'= Y' - 0.1873*U - 0.4681*V
    B'= Y' + 1.8556*U + 0.0000*V
    */
    // BT.709
    // *r = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.5748)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))))),    vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.5748)), vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y)))))))));
    // *g = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.1873)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))), vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.4681))))),                                 vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.1873)),vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y))))), vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.4681)))     ))));
    // *b = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.8556)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))))),    vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.8556)), vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y)))))))));

    // BT.601
    *r = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.403)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))))),
								 vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.403)), vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y)))))))));
    *g = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.344)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))), vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.714))))),
								 vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.344)),vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y))))), vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.714)))     ))));
    *b = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.773)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))))),
								 vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.773)), vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y)))))))));

    // Android nv21 conversion
    // *r = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.370705)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))))),
	// 							 vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.370705)), vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y)))))))));
    // *g = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.337633)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))), vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.698001))))),
	// 							 vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.337633)),vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y))))), vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cr)))), vdupq_n_f32(-128.0)), vdupq_n_f32(-0.698001)))     ))));
    // *b = vqmovn_u16(vcombine_u16(vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.732446)), vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(y))))))),
	// 							 vqmovn_u32(vcvtq_u32_f32(vaddq_f32(vmulq_f32(vaddq_f32(vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(cb)))), vdupq_n_f32(-128.0)), vdupq_n_f32(1.732446)), vcvtq_f32_u32(vmovl_u16(vget_high_u16(vmovl_u8(y)))))))));
    return;
}

static inline void vizpet_arm_neon::ics_yuv2rgb_bt709_neon(unsigned char y, unsigned char cb, unsigned char cr,
                          unsigned char *r, unsigned char *g, unsigned char *b){
    /*
    R'= Y' + 0.0000*U + 1.5748*V
    G'= Y' - 0.1873*U - 0.4681*V
    B'= Y' + 1.8556*U + 0.0000*V
    */

    double f_y = (double)y;
    double f_u = (double)cb - 128;
    double f_v = (double)cr - 128;

//https://software-dl.ti.com/jacinto7/esd/processor-sdk-rtos-jacinto7/06_02_00_21/exports/docs/vxlib_c66x_1_1_4_0/docs/doxygen/html/vxlib_html/group___v_x_l_i_b__color_convert___y_u_v_xto_r_g_b__i8u__o8u.html
    // BT.709
    // double f_r = f_y + 0.0000f*f_u + 1.5748f*f_v;
    // double f_g = f_y - 0.1873f*f_u - 0.4681f*f_v;
    // double f_b = f_y + 1.8556f*f_u + 0.0000f*f_v;

    // BT.601    
    double f_r = f_y + 1.403f * f_v;
    double f_g = f_y - 0.714f * f_v - 0.344f * f_u;
    double f_b = f_y + 1.773f * f_u;

    int i_r = (int)f_r;
    int i_g = (int)f_g;
    int i_b = (int)f_b;
    *r = vizpet_arm_neon::ics_usat8_neon(i_r);
    *g = vizpet_arm_neon::ics_usat8_neon(i_g);
    *b = vizpet_arm_neon::ics_usat8_neon(i_b);

    return;
}

static inline unsigned char vizpet_arm_neon::ics_usat8_neon(int a){
    if (a > 255)
        a = 255;
    if (a < 0)
        a = 0;
    return (unsigned char)a;
}

int vizpet_arm_neon::ics_fconvert_nv21torgb_neon(unsigned char* in, unsigned char* out, int width, int height){
    int in_stride = width;
    int width1   = (width/(16));
    width1 = (width1)*16;
    for (int y = 0; y < height; y+=2){
        int x = 0;int i=0;
        for (x = 0; x < width1; x+=16){
            uint8x8_t luma00  = vld1_u8(in + y*in_stride     + x);
            uint8x8_t luma10  = vld1_u8(in + (y+1)*in_stride + x);
            uint8x8_t luma01  = vld1_u8(in + y*in_stride     + x+8);
            uint8x8_t luma11  = vld1_u8(in + (y+1)*in_stride + x+8);
            uint8x8x2_t cbcr  = vld2_u8(in + width*height    + (y/2)*width + x);
            uint8x8x2_t cb;
            uint8x8x2_t cr;
            uint8x8x3_t bgr00;
            uint8x8x3_t bgr01;
            uint8x8x3_t bgr10;
            uint8x8x3_t bgr11;

            cb = vzip_u8(cbcr.val[1], cbcr.val[1]);
            cr = vzip_u8(cbcr.val[0], cbcr.val[0]);

            vizpet_arm_neon::ics_yuv2rgb_bt709_neon_8I(luma00, cb.val[0], cr.val[0], &bgr00.val[2], &bgr00.val[1], &bgr00.val[0]);
            vizpet_arm_neon::ics_yuv2rgb_bt709_neon_8I(luma01, cb.val[1], cr.val[1], &bgr01.val[2], &bgr01.val[1], &bgr01.val[0]);
            vizpet_arm_neon::ics_yuv2rgb_bt709_neon_8I(luma10, cb.val[0], cr.val[0], &bgr10.val[2], &bgr10.val[1], &bgr10.val[0]);
            vizpet_arm_neon::ics_yuv2rgb_bt709_neon_8I(luma11, cb.val[1], cr.val[1], &bgr11.val[2], &bgr11.val[1], &bgr11.val[0]);
            vst3_u8(out + y*width*3     + i*24,     bgr00);
            vst3_u8(out + (y+1)*width*3 + i*24,     bgr10);
            vst3_u8(out + y*width*3     + (i+1)*24, bgr01);
            vst3_u8(out + (y+1)*width*3 + (i+1)*24, bgr11);
            i+=2;
        }

        for (; x < width; x+=2){
            unsigned char* luma1 = in  + y*in_stride     + x;
            unsigned char* luma2 = in  + (y+1)*in_stride + x;
            unsigned char* cbcr  = in  + width*height    + (y/2)*width + x;
            unsigned char* bgr1  = out + y*width*3       + 3*x;
            unsigned char* bgr2  = out + (y+1)*width*3   + 3*x;
            vizpet_arm_neon::ics_yuv2rgb_bt709_neon(luma1[0], cbcr[1], cbcr[0], &bgr1[2], &bgr1[1], &bgr1[0]);
            vizpet_arm_neon::ics_yuv2rgb_bt709_neon(luma1[1], cbcr[1], cbcr[0], &bgr1[5], &bgr1[4], &bgr1[3]);
            vizpet_arm_neon::ics_yuv2rgb_bt709_neon(luma2[0], cbcr[1], cbcr[0], &bgr2[2], &bgr2[1], &bgr2[0]);
            vizpet_arm_neon::ics_yuv2rgb_bt709_neon(luma2[1], cbcr[1], cbcr[0], &bgr2[5], &bgr2[4], &bgr2[3]);
        }
    }

    return 0;
}
// void print_y_range(char* yuv, int w, int h){
//     int min_y = INT_MAX; int max_y = INT_MIN;
//     for(int i=0; i<w*h; i++){
//         if(yuv[i] < min_y) min_y = yuv[i];
//         else if(yuv[i] > max_y) max_y = yuv[i];
//     }
//     LOGV("VZ Debug: Y range: %d\t-\t%d", min_y, max_y);
// }
cv::Mat vizpet_arm_neon::nv21_resize_convert_rotate(char* yuv, const int image_width, int const image_height, int const target_width, int const target_height, int rotation_angle) {
    // resize
    char *yuv_resized = new char[target_height * 1.5 * target_width];
    vizpet_arm_neon::resize_nv21_buffer(yuv, yuv_resized, image_width, image_height, target_width, target_height);

    // convert
    cv::Mat image(target_height, target_width, CV_8UC3);
    unsigned char *bgr = (unsigned char*) image.data;
    vizpet_arm_neon::ics_fconvert_nv21torgb_neon(reinterpret_cast<unsigned char*>(yuv_resized), bgr, target_width, target_height);
    delete[] yuv_resized;
    // unsigned char *bgr = new unsigned char[image_height * image_width * 3];
    // vizpet_arm_neon::ics_fconvert_nv21torgb_neon(reinterpret_cast<unsigned char*>(yuv), bgr, image_width, image_height);
    // cv::Mat image(image_height, image_width, CV_8UC3, bgr);

    // rotate
    if(rotation_angle != 0)
        image = rotate_image(image, rotation_angle);
    return image;
}

cv::Mat vizpet_arm_neon::nv21_resize_convert_without_rotate(char* yuv, const int image_width, int const image_height, int const target_width, int const target_height, int rotation_angle) {
    // resize
    char *yuv_resized = new char[target_height * 1.5 * target_width];
    vizpet_arm_neon::resize_nv21_buffer(yuv, yuv_resized, image_width, image_height, target_width, target_height);

    // convert
    cv::Mat image(target_height, target_width, CV_8UC3);
    unsigned char *bgr = (unsigned char*) image.data;
    vizpet_arm_neon::ics_fconvert_nv21torgb_neon(reinterpret_cast<unsigned char*>(yuv_resized), bgr, target_width, target_height);
    delete[] yuv_resized;
    // unsigned char *bgr = new unsigned char[image_height * image_width * 3];
    // vizpet_arm_neon::ics_fconvert_nv21torgb_neon(reinterpret_cast<unsigned char*>(yuv), bgr, image_width, image_height);
    // cv::Mat image(image_height, image_width, CV_8UC3, bgr);

    // rotate
    return image;
}

cv::Mat vizpet_arm_neon::nv21_convert_resize_rotate(char* yuv, const int image_width, int const image_height, int const target_width, int const target_height, int rotation_angle) {
    // convert
    cv::Mat image(image_height, image_width, CV_8UC3);
    unsigned char *bgr = (unsigned char*) image.data;
    vizpet_arm_neon::ics_fconvert_nv21torgb_neon(reinterpret_cast<unsigned char*>(yuv), bgr, image_width, image_height);

    // resize
    if(!(target_width == image_width && target_height == image_height))
        cv::resize(image, image, {target_width, target_height});

    // rotate
    if(rotation_angle != 0)
        image = rotate_image(image, rotation_angle);
    return image;
}

#endif //ANDROID_ARM_NEON