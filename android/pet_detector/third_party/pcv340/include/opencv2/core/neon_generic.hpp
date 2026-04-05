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
// Copyright (C) 2013, Samsung Electronics, all rights reserved.
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
*/
/*******************************************************************************
 *
  * (C) Samsung DMC Design Solution Lab, Samsung Electronics 2015
 *
  * Scope of work: OpenCV Performance Optimization using NEON and OpenMP acceleration.
 *
 * Authors:
  *  - Cheol-Yong Jeon <cy77.jeon@samsung.com>
 *
 *******************************************************************************/


#ifndef _OPENCV_NEON_GENERIC_HPP_
#define _OPENCV_NEON_GENERIC_HPP_

/* Klocwork: the neon registers data type is not properly
 *           recognized during the analyze.All the functions
 *           which returns value are marked as implicit int, 
 *           what should be treated as false positive.
 */

//These functions provide a way to use NEON intrinsics without a need to specify parameter types in function name.
//This allows to create generic code that can be reused for many different types.

//This list is not complete, it should be extended when needed.

#include <cassert>
#if CV_NEON
namespace geneon{




//Conversions
//Output and input vector are passed as parameters to provide all needed information about types
//During conversion to low precision type, values are saturated
//Float values are rounded when converting to int

void vcvt(uint8x8_t& out, uint8x8_t a); //no computation
void vcvt(uint8x8_t& out, int8x8_t a);
void vcvt(uint8x8_t& out, uint16x8_t a);
void vcvt(uint8x8_t& out, int16x8_t a);
void vcvt(uint8x8_t& out, uint32x4x2_t a); //
void vcvt(uint8x8_t& out, int32x4x2_t a);
void vcvt(uint8x8_t& out, float32x4x2_t a);
void vcvt(int8x8_t& out, uint8x8_t a);
void vcvt(int8x8_t& out, int8x8_t a); //no computation
void vcvt(int8x8_t& out, uint16x8_t a);
void vcvt(int8x8_t& out, int16x8_t a);
void vcvt(int8x8_t& out, int32x4x2_t a);
void vcvt(int8x8_t& out, float32x4x2_t a); //
void vcvt(uint16x4x2_t& out, uint32x4x2_t a);
void vcvt(uint16x8_t& out, uint8x8_t a);
void vcvt(uint16x8_t& out, int8x8_t a);
void vcvt(uint16x8_t& out, uint16x8_t a); //no computation
void vcvt(uint16x8_t& out, int16x8_t a);
void vcvt(uint16x8_t& out, uint32x4x2_t a);
void vcvt(uint16x8_t& out, int32x4x2_t a);
void vcvt(uint16x8_t& out, float32x4x2_t a);
void vcvt(int16x8_t& out, uint8x8_t a);
void vcvt(int16x8_t& out, int8x8_t a);
void vcvt(int16x8_t& out, uint16x8_t a);
void vcvt(int16x8_t& out, int16x8_t a); //no computation
void vcvt(int16x8_t& out, int32x4x2_t a);
void vcvt(int16x8_t& out, float32x4x2_t a);
void vcvt(uint32x4x2_t& out, uint8x8_t a);
void vcvt(uint32x4x2_t& out, uint16x8_t a);
void vcvt(uint32x4x2_t& out, uint32x4x2_t a); //no computation
void vcvt(uint32x4x2_t& out, float32x4x2_t a); //no computation
void vcvt(int32x4x2_t& out, uint8x8_t a);
void vcvt(int32x4x2_t& out, int8x8_t a);
void vcvt(int32x4x2_t& out, uint16x8_t a);
void vcvt(int32x4x2_t& out, int16x8_t a);
void vcvt(int32x4x2_t& out, uint32x4x2_t a);
void vcvt(int32x4x2_t& out, int32x4x2_t a); //no computation
void vcvt(int32x4x2_t& out, float32x4x2_t a);
void vcvt(float32x4x2_t& out, uint8x8_t a);
void vcvt(float32x4x2_t& out, int8x8_t a);
void vcvt(float32x4x2_t& out, uint16x8_t a);
void vcvt(float32x4x2_t& out, int16x8_t a);
void vcvt(float32x4x2_t& out, int32x4x2_t a);
void vcvt(float32x4x2_t& out, uint32x4x2_t a);
void vcvt(float32x4x2_t& out, float32x4x2_t a); //no computation

inline void vcvt(uint8x8_t& out, uint8x8_t a){
    out = a;
}
inline void vcvt(uint8x8_t& out, int8x8_t a){
    out = vreinterpret_u8_s8(vmax_s8(a, vdup_n_s8(0)));
}
inline void vcvt(uint8x8_t& out, uint16x8_t a){
    out = vqmovn_u16(a);
}
inline void vcvt(uint8x8_t& out, int16x8_t a){
    out = vqmovun_s16(a);
}
inline void vcvt(uint8x8_t& out, uint32x4x2_t a){
    uint16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(uint8x8_t& out, int32x4x2_t a){
    uint16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(uint8x8_t& out, float32x4x2_t a){
    uint32x4_t b, c;
    uint16x4x2_t d;
    uint16x8_t e;
    b = vcvtq_u32_f32( a.val[0] );
    c = vcvtq_u32_f32( a.val[1] );
    d.val[0] = vqmovn_u32( b );
    d.val[1] = vqmovn_u32( c );
    e = vcombine_u16( d.val[0], d.val[1] );
    out = vqmovn_u16( e );

    //uint16x8_t b;
    //vcvt(b, a);
    //vcvt(out, b);
}
inline void vcvt(int8x8_t& out, uint8x8_t a){
    out = vreinterpret_s8_u8(vmin_u8(a, vdup_n_u8(0x7F)));
}
inline void vcvt(int8x8_t& out, int8x8_t a){
    out = a;
}
inline void vcvt(int8x8_t& out, uint16x8_t a){
    uint8x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(int8x8_t& out, int16x8_t a){
    out = vqmovn_s16(a);
}
inline void vcvt(int8x8_t& out, int32x4x2_t a){
    int16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(int8x8_t& out, float32x4x2_t a){
    int32x4x2_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(uint16x4x2_t& out, uint32x4x2_t a) {
    out.val[0] = vqmovn_u32( a.val[0] );
    out.val[1] = vqmovn_u32( a.val[1] );
}
inline void vcvt(uint16x8_t& out, uint8x8_t a){
    out = vmovl_u8(a);
}
inline void vcvt(uint16x8_t& out, int8x8_t a){
    uint8x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(uint16x8_t& out, uint16x8_t a){
    out = a;
}
inline void vcvt(uint16x8_t& out, int16x8_t a){
    out = vreinterpretq_u16_s16(vmaxq_s16(a, vdupq_n_s16(0)));
}
inline void vcvt(uint16x8_t& out, uint32x4x2_t a){
    out = vcombine_u16(vqmovn_u32(a.val[0]),vqmovn_u32(a.val[1]));
}
inline void vcvt(uint16x8_t& out, int32x4x2_t a){
    uint16x4_t b = vqmovun_s32(a.val[0]);
    uint16x4_t c = vqmovun_s32(a.val[1]);
    //out = vcombine_u16(vqmovun_s32(a.val[0]),vqmovun_s32(a.val[1]));
    out = vcombine_u16(b, c);
}
inline void vcvt(uint16x8_t& out, float32x4x2_t a){
    int32x4x2_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(int16x8_t& out, uint8x8_t a){
    uint16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(int16x8_t& out, int8x8_t a){
    out = vmovl_s8(a);
}
inline void vcvt(int16x8_t& out, uint16x8_t a){
    out = vreinterpretq_s16_u16(vminq_u16(a, vdupq_n_u16(0x7FFF)));
}
inline void vcvt(int16x8_t& out, int16x8_t a){
    out = a;
}
inline void vcvt(int16x8_t& out, int32x4x2_t a){
    out = vcombine_s16(vqmovn_s32(a.val[0]),vqmovn_s32(a.val[1]));
}
inline void vcvt(int16x8_t& out, float32x4x2_t a){
    int32x4x2_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(uint32x4x2_t& out, uint8x8_t a){
    uint16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(uint32x4x2_t& out, uint16x8_t a){
    out.val[0] = vmovl_u16(vget_low_u16(a));
    out.val[1] = vmovl_u16(vget_high_u16(a));
}
inline void vcvt(uint32x4x2_t& out, uint32x4x2_t a){
    out = a;
}
inline void vcvt(uint32x4x2_t& out, int32x4x2_t a){
    out.val[0] = vreinterpretq_u32_s32( a.val[0] );
    out.val[1] = vreinterpretq_u32_s32( a.val[1] );
}
inline void vcvt(uint32x4x2_t& out, float32x4x2_t a){
    out.val[0] = vcvtq_u32_f32( a.val[0] );
    out.val[1] = vcvtq_u32_f32( a.val[1] );
}
inline void vcvt(int32x4x2_t& out, uint8x8_t a){
    uint16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(int32x4x2_t& out, int8x8_t a){
    int16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(int32x4x2_t& out, uint16x8_t a){
    uint32x4x2_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(int32x4x2_t& out, int16x8_t a){
    out.val[0] = vmovl_s16(vget_low_s16(a));
    out.val[1] = vmovl_s16(vget_high_s16(a));
}
inline void vcvt(int32x4x2_t& out, uint32x4x2_t a){
    out.val[0] = vreinterpretq_s32_u32(vminq_u32(a.val[0], vdupq_n_u32(0x7fffffff)));
    out.val[1] = vreinterpretq_s32_u32(vminq_u32(a.val[1], vdupq_n_u32(0x7fffffff)));
}
inline void vcvt(int32x4x2_t& out, int32x4x2_t a){
    out = a;
}
inline void vcvt(int32x4x2_t& out, float32x4x2_t a){
    out.val[0] = vcvtq_s32_f32 (a.val[0]);
    out.val[1] = vcvtq_s32_f32 (a.val[1]);
}
inline void vcvt(float32x4x2_t& out, uint8x8_t a){
    uint16x8_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(float32x4x2_t& out, int8x8_t a){
    int32x4x2_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(float32x4x2_t& out, uint16x8_t a){
    uint32x4x2_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(float32x4x2_t& out, int16x8_t a){
    int32x4x2_t b;
    vcvt(b, a);
    vcvt(out, b);
}
inline void vcvt(float32x4x2_t& out, uint32x4x2_t a){
    out.val[0] = vcvtq_f32_u32 (a.val[0]);
    out.val[1] = vcvtq_f32_u32 (a.val[1]);
}
inline void vcvt(float32x4x2_t& out, int32x4x2_t a){
    out.val[0] = vcvtq_f32_s32 (a.val[0]);
    out.val[1] = vcvtq_f32_s32 (a.val[1]);
}
inline void vcvt(float32x4x2_t& out, float32x4x2_t a){
    out = a;
}
//Conversions for 4 element vectors
inline int8x8_t vcvt(int16x4_t a, int16x4_t b){
    return vqmovn_s16 (vcombine_s16 (a, b));
}
inline uint8x8_t vcvt(uint16x4_t a, uint16x4_t b){
    return vqmovn_u16 (vcombine_u16 (a, b));
}
inline void vcvt(int16x4_t& out, float32x4_t a){
    out = vqmovn_s32 (vcvtq_s32_f32(a));
}
inline void vcvt(uint16x4_t& out, float32x4_t a){
    out = vqmovun_s32 (vcvtq_s32_f32(a));
}
//Conversions with 2 output vectors
inline void vcvt(float32x4_t& out1, float32x4_t& out2, int16x8_t a){
    out1 = vcvtq_f32_s32 (vmovl_s16(vget_low_s16(a)));
    out2 = vcvtq_f32_s32 (vmovl_s16(vget_high_s16(a)));
}
inline void vcvt(float32x4_t& out1, float32x4_t& out2, uint16x8_t a){
    out1 = vcvtq_f32_u32 (vmovl_u16(vget_low_u16(a)));
    out2 = vcvtq_f32_u32 (vmovl_u16(vget_high_u16(a)));
}
inline void vcvt(float32x4_t& out1, float32x4_t& out2, int32x4x2_t a){
    out1 = vcvtq_f32_s32 (a.val[0]);
    out2 = vcvtq_f32_s32 (a.val[1]);
}
inline void vcvt(float32x4_t& out1, float32x4_t& out2, int8x8_t a){
    int16x8_t b;
    vcvt(b, a);
    vcvt(out1, out2, b);
}
inline void vcvt(float32x4_t& out1, float32x4_t& out2, uint8x8_t a){
    uint16x8_t b;
    vcvt(b, a);
    vcvt(out1, out2, b);
}
//Conversions with 2 input vectors
inline void vcvt(uint8x8_t& out, float32x4_t a, float32x4_t b){
    uint16x4_t a1;
    uint16x4_t b1;
    vcvt(a1, a);
    vcvt(b1, b);
    uint16x8_t c2;
    c2 = vcombine_u16(a1, b1);
    out = vqmovn_u16 (c2);
}
inline void vcvt(int8x8_t& out, float32x4_t a, float32x4_t b){
    int16x4_t a1;
    int16x4_t b1;
    vcvt(a1, a);
    vcvt(b1, b);
    int16x8_t c2;
    c2 = vcombine_s16(a1, b1);
    out = vqmovn_s16 (c2);
}
inline void vcvt(uint16x8_t& out, float32x4_t a, float32x4_t b){
    uint16x4_t a1;
    uint16x4_t b1;
    vcvt(a1, a);
    vcvt(b1, b);
    out = vcombine_u16 (a1, b1);
}
inline void vcvt(int16x8_t& out, float32x4_t a, float32x4_t b){
    int16x4_t a1;
    int16x4_t b1;
    vcvt(a1, a);
    vcvt(b1, b);
    out = vcombine_s16 (a1, b1);
}
inline void vcvt(int32x4x2_t& out, float32x4_t a, float32x4_t b){
    out.val[0] = vcvtq_s32_f32(a);
    out.val[1] = vcvtq_s32_f32(b);
}
inline void vcvt(float32x4x2_t& out, float32x4_t a, float32x4_t b){
    out.val[0] = a;
    out.val[1] = b;
}
inline void vcvt(float32x4_t& out1, float32x4_t& out2, float32x4x2_t a){
    out1 = a.val[0];
    out2 = a.val[1];
}


//Set all lanes to same value
//Output vector is passed as output parameter
inline void vmov_n(uint8x8_t& out, uint8_t value){
    out = vmov_n_u8 (value);
}
inline void vmov_n(int8x8_t& out, int8_t value){
    out = vmov_n_s8 (value);
}
inline void vmov_n(uint16x4_t& out, uint16_t value){
    out = vmov_n_u16 (value);
}
inline void vmov_n(int16x4_t& out, int16_t value){
    out = vmov_n_s16 (value);
}
inline void vmov_n(uint16x8_t& out, uint16_t value){
    out = vmovq_n_u16 (value);
}
inline void vmov_n(int16x8_t& out, int16_t value){
    out = vmovq_n_s16 (value);
}
inline void vmov_n(uint32x4_t& out, uint32_t value){
    out = vmovq_n_u32 (value);
}
inline void vmov_n(int32x4_t& out, int32_t value){
    out = vmovq_n_s32 (value);
}
inline void vmov_n(uint32x4x2_t& out, uint32_t value){
    out.val[0] = vmovq_n_u32(value);
    out.val[1] = vmovq_n_u32(value);
}
inline void vmov_n(int32x4x2_t& out, int32_t value){
    out.val[0] = vmovq_n_s32(value);
    out.val[1] = vmovq_n_s32(value);
}
inline void vmov_n(uint64x2_t& out, uint64_t value){
    out = vmovq_n_u64 (value);
}
inline void vmov_n(int64x2_t& out, int64_t value){
    out = vmovq_n_s64 (value);
}
inline void vmov_n(uint64x2x2_t& out, uint64_t value){
    out.val[0] = vmovq_n_u64(value);
    out.val[1] = vmovq_n_u64(value);
}
inline void vmov_n(int64x2x2_t& out, int64_t value){
    out.val[0] = vmovq_n_s64(value);
    out.val[1] = vmovq_n_s64(value);
}
inline void vmov_n(float32x2_t& out, float value){
    out = vmov_n_f32 (value);
}
inline void vmov_n(float32x4_t& out, float value){
    out = vmovq_n_f32 (value);
}
inline void vmov_n(float32x4x2_t& out, float value){
    out.val[0] = vmovq_n_f32 (value);
    out.val[1] = vmovq_n_f32 (value);
}


//Loads and stores. In case of vld1, in order to provide enough information about type,
//result vector is passed as output parameter instead of being a return value.

// Generic example for load and store. This example loads 8 elements of type T into vector r0.
// Then conversion is performed and resulting 8 element vector r1 of type DT is stored.
//
//for( ; x <= size.width - 8; x += 8 )
//{
//    typename neon<T>::size8 r0;
//    vld1 (r0, (const T *)(src + x));
//
//    typename neon<DT>::size8 r1;
//    vcvt(r1, r0);
//
//    vst1 ((DT *)(dst + x), r1);
//}

//Other example, converting uchar to ushort:
//
//neon<uchar>::size_d r0;
//neon<ushort>::size::d::size_q r1;
//vld1 (r0, (const uchar *)(src));
//vcvt(r1, r0);
//vst1 ((ushort *)(dst), r1);

inline void vld3(uint8x8x3_t& out, const uint8_t *addr){
    out = vld3_u8(addr);
}


inline void vld1(uint8x8_t& out, const uint8_t *addr){
    out = vld1_u8(addr);
}
inline void vld1(int8x8_t& out, const int8_t *addr){
    out = vld1_s8(addr);
}
inline void vld1(uint8x16_t& out, const uint8_t *addr){
    out = vld1q_u8(addr);
}
inline void vld1(int8x16_t& out, const int8_t *addr){
    out = vld1q_s8(addr);
}
inline void vld1(uint16x4_t& out, const uint16_t *addr){
    out = vld1_u16(addr);
}
inline void vld1(int16x4_t& out, const int16_t *addr){
    out = vld1_s16(addr);
}
inline void vld1(uint16x8_t& out, const uint16_t *addr){
    out = vld1q_u16(addr);
}
inline void vld1(int16x8_t& out, const int16_t *addr){
    out = vld1q_s16(addr);
}
inline void vld1(uint32x2_t& out, const uint32_t *addr){
    out = vld1_u32(addr);
}
inline void vld1(int32x2_t& out, const int32_t *addr){
    out = vld1_s32(addr);
}
inline void vld1(uint32x4_t& out, const uint32_t *addr){
    out = vld1q_u32(addr);
}
inline void vld1(int32x4_t& out, const int32_t *addr){
    out = vld1q_s32(addr);
}
inline void vld1(int32x4x2_t& out, const int32_t *addr){
    out.val[0] = vld1q_s32(addr);
    out.val[1] = vld1q_s32(addr + 4);
}
inline void vld1(uint32x4x2_t& out, const uint32_t *addr){
    out.val[0] = vld1q_u32(addr);
    out.val[1] = vld1q_u32(addr + 4);
}
inline void vld1(float32x2_t& out, const float32_t *addr){
    out = vld1_f32(addr);
}
inline void vld1(float32x4_t& out, const float32_t *addr){
    out = vld1q_f32(addr);
}
inline void vld1(float32x4x2_t& out, const float32_t *addr){
    out.val[0] = vld1q_f32(addr);
    out.val[1] = vld1q_f32(addr + 4);
}


//Stores
inline void vst1(uint8_t *dst, uint8x8_t a){
    vst1_u8 (dst , a);
}
inline void vst1(int8_t *dst, int8x8_t a){
    vst1_s8 (dst, a);
}
inline void vst1(uint8_t *dst, uint8x16_t a){
    vst1q_u8 (dst , a);
}
inline void vst1(int8_t *dst, int8x16_t a){
    vst1q_s8 (dst, a);
}
inline void vst1(uint16_t *dst, uint16x4_t a){
    vst1_u16(dst, a);
}
inline void vst1(int16_t *dst, int16x4_t a){
    vst1_s16(dst, a);
}
inline void vst1(uint16_t *dst, uint16x4x2_t a){
    vst1_u16 (dst , a.val[0]);
    vst1_u16 (dst+4 , a.val[1]);
}
inline void vst1(uint16_t *dst, uint16x8_t a){
    vst1q_u16 (dst , a);
}
inline void vst1(int16_t *dst, int16x8_t a){
    vst1q_s16 (dst, a);
}
inline void vst1(uint32_t *dst, uint32x4_t a){
    vst1q_u32(dst, a);
}
inline void vst1(uint32_t *dst, uint32x4x2_t a){
    vst1q_u32(dst, a.val[0]);
    vst1q_u32(dst+4, a.val[1]);
}
inline void vst1(int32_t *dst, int32x4_t a){
    vst1q_s32(dst, a);
}
inline void vst1(int32_t *dst, int32x4x2_t a){
    vst1q_s32 (dst, a.val[0]);
    vst1q_s32 (dst+4, a.val[1]);
}
inline void vst1(float32_t *dst, float32x2_t a){
    vst1_f32(dst, a);
}
inline void vst1(float32_t *dst, float32x4_t a){
    vst1q_f32(dst, a);
}
inline void vst1(float32_t *dst, float32x4x2_t a){
    vst1q_f32 (dst, a.val[0]);
    vst1q_f32 (dst+4, a.val[1]);
}

/**
 * Defines for one argument function
 */

//Generic function definition for vector sizes 16x4 and one argument
#define NEON_GENERIC_ONE_ARG_16x4(func, ret_type, type, suffix) \
inline ret_type func(type a){ \
    return func##_##suffix(a); }

#define NEON_GENERIC_ONE_ARG_32x4(func, ret_type, type, suffix) \
inline ret_type func(type a){ \
    return func##q_##suffix(a); }


#define NEON_GENERIC_ONE_ARG_8x8(func, ret_type, type,  suffix) \
inline ret_type func(type a){ \
    return func##_##suffix(a); }

#define NEON_GENERIC_ONE_ARG_16x8(func, ret_type, type, suffix) \
inline ret_type func(type a){ \
    return func##q_##suffix(a); }

#define NEON_GENERIC_ONE_ARG_32x8(func, ret_type, type, suffix) \
inline ret_type func(type a){ \
    ret_type out; \
    out.val[0] = func##q_##suffix(a.val[0]); \
    out.val[1] = func##q_##suffix(a.val[1]); \
    return out; }

//Generic function definitions for signed parameters
#define NEON_GENERIC_ONE_ARG_S(func) \
NEON_GENERIC_ONE_ARG_8x8(func, int8x8_t, int8x8_t, s8) \
NEON_GENERIC_ONE_ARG_16x8(func, int16x8_t, int16x8_t, s16) \
NEON_GENERIC_ONE_ARG_32x8(func, int32x4x2_t, int32x4x2_t, s32)

//Generic function definitions for unsigned parameters
#define NEON_GENERIC_ONE_ARG_U(func) \
NEON_GENERIC_ONE_ARG_8x8(func, uint8x8_t, uint8x8_t, u8) \
NEON_GENERIC_ONE_ARG_16x8(func, uint16x8_t, uint16x8_t, u16) \
NEON_GENERIC_ONE_ARG_32x8(func, uint32x4x2_t, uint32x4x2_t, u32)

//Generic function definitions for float parameters
#define NEON_GENERIC_ONE_ARG_F(func) \
NEON_GENERIC_ONE_ARG_32x8(func, float32x4x2_t, float32x4x2_t, f32)

//Generic function definitions for vget_low, vget_high
#define NEON_GENERIC_ONE_ARG_VGET(func) \
NEON_GENERIC_ONE_ARG_8x8(func, int8x8_t, int8x16_t, s8) \
NEON_GENERIC_ONE_ARG_8x8(func, int16x4_t, int16x8_t, s16) \
NEON_GENERIC_ONE_ARG_8x8(func, int32x2_t, int32x4_t, s32) \
NEON_GENERIC_ONE_ARG_8x8(func, int64x1_t, int64x2_t, s64) \
NEON_GENERIC_ONE_ARG_8x8(func, uint8x8_t, uint8x16_t, u8) \
NEON_GENERIC_ONE_ARG_8x8(func, uint16x4_t, uint16x8_t, u16) \
NEON_GENERIC_ONE_ARG_8x8(func, uint32x2_t, uint32x4_t, u32) \
NEON_GENERIC_ONE_ARG_8x8(func, uint64x1_t, uint64x2_t, u64) \
NEON_GENERIC_ONE_ARG_8x8(func, float32x2_t, float32x4_t, f32)

NEON_GENERIC_ONE_ARG_VGET( vget_low )
NEON_GENERIC_ONE_ARG_VGET( vget_high )


/**
 * Defines for two arguments function
 */

//Generic definitions for functions with 2 arguments
#define NEON_GENERIC_TWO_ARGS_64x1(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_32x2(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_64x2(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##q_##suffix(a, b); }


#define NEON_GENERIC_TWO_ARGS_16x4(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_32x4(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##q_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_8x8(func, ret_type, type_a, type_b,  suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_16x8(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##q_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_32x8(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    ret_type out; \
    out.val[0] = func##q_##suffix(a.val[0], b.val[0]); \
    out.val[1] = func##q_##suffix(a.val[1], b.val[1]); \
    return out; }

#define NEON_GENERIC_TWO_ARGS_32x8_32x4(func, ret_type, type_a, type_b, suffix) \
inline ret_type func(type_a a, type_b b){ \
    ret_type out; \
    out.val[0] = func##q_##suffix(a.val[0], b); \
    out.val[1] = func##q_##suffix(a.val[1], b); \
    return out; }


#define NEON_GENERIC_TWO_ARGS_8x16(func, ret_type, type_a, type_b,  suffix) \
inline ret_type func(type_a a, type_b b){ \
    return func##q_##suffix(a, b); }


// Definitions for functions which need to call intrinsics with literals.
// For an example, vget_lane_ series intriniscs
#define NEON_GENERIC_TWO_ARGS_64x1_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_32x2_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_64x2_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##q_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_16x4_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_32x4_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##q_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_8x8_TEMPLATE(func, ret_type, type_a,  suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_16x8_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##q_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_32x8_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b1, int b2> \
inline ret_type func(type_a a){ \
    ret_type out; \
    out.val[0] = func##q_##suffix(a.val[0], b1); \
    out.val[1] = func##q_##suffix(a.val[1], b2); \
    return out; }

#define NEON_GENERIC_TWO_ARGS_8x16_TEMPLATE(func, ret_type, type_a, suffix) \
template<int b> \
inline ret_type func(type_a a){ \
    return func##q_##suffix(a, b); }

#define NEON_GENERIC_TWO_ARGS_INT(func) \
NEON_GENERIC_TWO_ARGS_8x8(func, uint8x8_t, uint8x8_t, uint8x8_t, u8) \
NEON_GENERIC_TWO_ARGS_8x8(func, int8x8_t, int8x8_t, int8x8_t, s8) \
NEON_GENERIC_TWO_ARGS_8x16(func, uint8x16_t, uint8x16_t, uint8x16_t, u8) \
NEON_GENERIC_TWO_ARGS_8x16(func, int8x16_t, int8x16_t, int8x16_t, s8) \
NEON_GENERIC_TWO_ARGS_16x4(func, uint16x4_t, uint16x4_t, uint16x4_t, u16) \
NEON_GENERIC_TWO_ARGS_16x4(func, int16x4_t, int16x4_t, int16x4_t, s16) \
NEON_GENERIC_TWO_ARGS_16x8(func, uint16x8_t, uint16x8_t, uint16x8_t, u16) \
NEON_GENERIC_TWO_ARGS_16x8(func, int16x8_t, int16x8_t, int16x8_t, s16) \
NEON_GENERIC_TWO_ARGS_32x2(func, uint32x2_t, uint32x2_t, uint32x2_t, u32) \
NEON_GENERIC_TWO_ARGS_32x2(func, int32x2_t, int32x2_t, int32x2_t, s32) \
NEON_GENERIC_TWO_ARGS_32x4(func, uint32x4_t, uint32x4_t, uint32x4_t, u32) \
NEON_GENERIC_TWO_ARGS_32x4(func, int32x4_t, int32x4_t, int32x4_t, s32) \
NEON_GENERIC_TWO_ARGS_32x8(func, uint32x4x2_t, uint32x4x2_t, uint32x4x2_t, u32) \
NEON_GENERIC_TWO_ARGS_32x8(func, int32x4x2_t, int32x4x2_t, int32x4x2_t, s32)

#define NEON_GENERIC_TWO_ARGS_INT_LONG(func) \
NEON_GENERIC_TWO_ARGS_8x8(func, uint16x8_t, uint8x8_t, uint8x8_t, u8) \
NEON_GENERIC_TWO_ARGS_8x8(func, int16x8_t, int8x8_t, int8x8_t, s8) \
NEON_GENERIC_TWO_ARGS_16x4(func, uint32x4_t, uint16x4_t, uint16x4_t, u16) \
NEON_GENERIC_TWO_ARGS_16x4(func, int32x4_t, int16x4_t, int16x4_t, s16) \
NEON_GENERIC_TWO_ARGS_32x2(func, uint64x2_t, uint32x2_t, uint32x2_t, u32) \
NEON_GENERIC_TWO_ARGS_32x2(func, int64x2_t, int32x2_t, int32x2_t, s32)


#define NEON_GENERIC_TWO_ARGS_FLOAT(func) \
NEON_GENERIC_TWO_ARGS_32x2(func, float32x2_t, float32x2_t, float32x2_t, f32) \
NEON_GENERIC_TWO_ARGS_32x4(func, float32x4_t, float32x4_t, float32x4_t, f32) \
NEON_GENERIC_TWO_ARGS_32x8(func, float32x4x2_t, float32x4x2_t, float32x4x2_t, f32) \
NEON_GENERIC_TWO_ARGS_32x8_32x4(func, float32x4x2_t, float32x4x2_t, float32x4_t, f32)

#define NEON_GENERIC_TWO_ARGS_64(func) \
NEON_GENERIC_TWO_ARGS_64x1(func, uint64x1_t, uint64x1_t, uint64x1_t, u64) \
NEON_GENERIC_TWO_ARGS_64x1(func, int64x1_t, int64x1_t, int64x1_t, u64) \
NEON_GENERIC_TWO_ARGS_64x2(func, uint64x2_t, uint64x2_t, uint64x2_t, u64) \
NEON_GENERIC_TWO_ARGS_64x2(func, int64x2_t, int64x2_t, int64x2_t, s64)


#define NEON_GENERIC_TWO_ARGS_CMP(func) \
NEON_GENERIC_TWO_ARGS_8x8(func, uint8x8_t, uint8x8_t, uint8x8_t, u8) \
NEON_GENERIC_TWO_ARGS_8x8(func, uint8x8_t, int8x8_t, int8x8_t, s8) \
NEON_GENERIC_TWO_ARGS_16x4(func, uint16x4_t, uint16x4_t, uint16x4_t, u16) \
NEON_GENERIC_TWO_ARGS_16x4(func, uint16x4_t, int16x4_t, int16x4_t, s16) \
NEON_GENERIC_TWO_ARGS_16x8(func, uint16x8_t, uint16x8_t, uint16x8_t, u16) \
NEON_GENERIC_TWO_ARGS_16x8(func, uint16x8_t, int16x8_t, int16x8_t, s16) \
NEON_GENERIC_TWO_ARGS_32x2(func, uint32x2_t, uint32x2_t, uint32x2_t, u32) \
NEON_GENERIC_TWO_ARGS_32x2(func, uint32x2_t, int32x2_t, int32x2_t, s32) \
NEON_GENERIC_TWO_ARGS_32x2(func, uint32x2_t, float32x2_t, float32x2_t, f32) \
NEON_GENERIC_TWO_ARGS_32x4(func, uint32x4_t, uint32x4_t, uint32x4_t, u32) \
NEON_GENERIC_TWO_ARGS_32x4(func, uint32x4_t, int32x4_t, int32x4_t, s32) \
NEON_GENERIC_TWO_ARGS_32x4(func, uint32x4_t, float32x4_t, float32x4_t, f32) \
NEON_GENERIC_TWO_ARGS_32x8(func, uint32x4x2_t, uint32x4x2_t, uint32x4x2_t, u32) \
NEON_GENERIC_TWO_ARGS_32x8(func, uint32x4x2_t, int32x4x2_t, int32x4x2_t, s32) \
NEON_GENERIC_TWO_ARGS_32x8(func, uint32x4x2_t, float32x4x2_t, float32x4x2_t, f32)


/**
 * Defines for three arguments function
 */

// generic NEON function accepting 3 parameters as QUADWORDSx2
#define NEON_GENERIC_THREE_ARGS_Qx2(func, ret_type, type_a, type_b, type_c, suffix) \
    inline ret_type func(type_a a, type_b b, type_c c){     \
    type_b t; \
    t.val[0] = func##q_##suffix( a.val[0], b.val[0], c.val[0] ); \
    t.val[1] = func##q_##suffix( a.val[1], b.val[1], c.val[1] ); \
    return t; }

// generic NEON function accepting 3 parameters as QUADWORDS
#define NEON_GENERIC_THREE_ARGS_Q(func, ret_type, type_a, type_b, type_c, suffix) \
    inline ret_type func(type_a a, type_b b, type_c c){     \
    return func##q_##suffix(a, b, c); }

// generic NEON function accepting 3 parameters as DOUBLEWORD
#define NEON_GENERIC_THREE_ARGS_D(func, ret_type, type_a, type_b, type_c, suffix) \
    inline ret_type func(type_a a, type_b b, type_c c){     \
    return func##_##suffix(a, b, c); }

// Lists the VBSL intrinsics
#define NEON_GENERIC_VBSL(func)                     \
    NEON_GENERIC_THREE_ARGS_D(func,int8x8_t, uint8x8_t , int8x8_t, int8x8_t, s8) \
    NEON_GENERIC_THREE_ARGS_D(func,int16x4_t, uint16x4_t, int16x4_t, int16x4_t, s16) \
    NEON_GENERIC_THREE_ARGS_D(func,int32x2_t, uint32x2_t, int32x2_t, int32x2_t, s32) \
    NEON_GENERIC_THREE_ARGS_D(func,int64x1_t, uint64x1_t, int64x1_t, int64x1_t, s64) \
    NEON_GENERIC_THREE_ARGS_D(func,uint8x8_t, uint8x8_t, uint8x8_t, uint8x8_t, u8) \
    NEON_GENERIC_THREE_ARGS_D(func,uint16x4_t, uint16x4_t, uint16x4_t, uint16x4_t, u16) \
    NEON_GENERIC_THREE_ARGS_D(func,uint32x2_t , uint32x2_t, uint32x2_t, uint32x2_t, u32) \
    NEON_GENERIC_THREE_ARGS_D(func,uint64x1_t, uint64x1_t, uint64x1_t, uint64x1_t, u64) \
    NEON_GENERIC_THREE_ARGS_D(func,float32x2_t, uint32x2_t, float32x2_t, float32x2_t, f32) \
    NEON_GENERIC_THREE_ARGS_D(func,poly8x8_t, uint8x8_t, poly8x8_t, poly8x8_t, p8) \
    NEON_GENERIC_THREE_ARGS_D(func,poly16x4_t, uint16x4_t, poly16x4_t, poly16x4_t, p16) \
    NEON_GENERIC_THREE_ARGS_Q(func,int8x16_t, uint8x16_t , int8x16_t, int8x16_t, s8) \
    NEON_GENERIC_THREE_ARGS_Q(func,int16x8_t, uint16x8_t, int16x8_t, int16x8_t, s16) \
    NEON_GENERIC_THREE_ARGS_Q(func,int32x4_t, uint32x4_t, int32x4_t, int32x4_t, s32) \
    NEON_GENERIC_THREE_ARGS_Q(func,int64x2_t, uint64x2_t, int64x2_t, int64x2_t, s64) \
    NEON_GENERIC_THREE_ARGS_Q(func,uint8x16_t, uint8x16_t, uint8x16_t, uint8x16_t, u8) \
    NEON_GENERIC_THREE_ARGS_Q(func,uint16x8_t, uint16x8_t, uint16x8_t, uint16x8_t, u16) \
    NEON_GENERIC_THREE_ARGS_Q(func,uint32x4_t , uint32x4_t, uint32x4_t, uint32x4_t, u32) \
    NEON_GENERIC_THREE_ARGS_Q(func,uint64x2_t, uint64x2_t, uint64x2_t, uint64x2_t, u64) \
    NEON_GENERIC_THREE_ARGS_Q(func,float32x4_t, uint32x4_t, float32x4_t, float32x4_t, f32) \
    NEON_GENERIC_THREE_ARGS_Q(func,poly8x16_t, uint8x16_t, poly8x16_t, poly8x16_t, p8) \
    NEON_GENERIC_THREE_ARGS_Q(func,poly16x8_t, uint16x8_t, poly16x8_t, poly16x8_t, p16) \
    NEON_GENERIC_THREE_ARGS_Qx2(func,uint32x4x2_t, uint32x4x2_t, uint32x4x2_t, uint32x4x2_t, u32) \
    NEON_GENERIC_THREE_ARGS_Qx2(func,int32x4x2_t, uint32x4x2_t, int32x4x2_t, int32x4x2_t, s32) \
    NEON_GENERIC_THREE_ARGS_Qx2(func,float32x4x2_t, uint32x4x2_t, float32x4x2_t, float32x4x2_t, f32)

#define NEON_GENERIC_VREINTERPRET_D( func, dst_type, src_type, suffix_dst, suffix_src ) \
    inline void func(dst_type & b, src_type a){     \
        b = vreinterpret##_##suffix_dst##_##suffix_src( a ); }

#define NEON_GENERIC_VREINTERPRET_Q( func, dst_type, src_type, suffix_dst, suffix_src ) \
    inline void func(dst_type & b, src_type a){     \
        b = vreinterpretq##_##suffix_dst##_##suffix_src( a ); }

#define NEON_GENERIC_CAST(func) \
    NEON_GENERIC_VREINTERPRET_D( func, uint8x8_t, int8x8_t, u8, s8 ) \
    NEON_GENERIC_VREINTERPRET_D( func, int8x8_t, uint8x8_t, s8, u8 ) \
    NEON_GENERIC_VREINTERPRET_Q( func, uint16x8_t, int16x8_t, u16, s16 ) \
    NEON_GENERIC_VREINTERPRET_Q( func, int16x8_t, uint16x8_t, s16, u16 ) \
    NEON_GENERIC_VREINTERPRET_Q( func, int32x4_t, uint16x8_t, s32, u16 )

NEON_GENERIC_CAST( vcast )
inline void vcast(int32x4x2_t & b, uint32x4x2_t a){ 
    b.val[0] = vreinterpretq_s32_u32( a.val[0] );
    b.val[1] = vreinterpretq_s32_u32( a.val[1] );
}
inline void vcast(int32x4x2_t & b, int32x4x2_t a){ 
    b.val[0] = a.val[0];
    b.val[1] = a.val[1];
}


//Absolute value
NEON_GENERIC_ONE_ARG_S(vabs)
NEON_GENERIC_ONE_ARG_S(vqabs)
NEON_GENERIC_ONE_ARG_F(vabs)

//Negation
NEON_GENERIC_ONE_ARG_S(vneg)
NEON_GENERIC_ONE_ARG_S(vqneg)
NEON_GENERIC_ONE_ARG_F(vneg)

//Bitwise not
NEON_GENERIC_ONE_ARG_S(vmvn)
NEON_GENERIC_ONE_ARG_U(vmvn)

//Count leading sign bits
NEON_GENERIC_ONE_ARG_S(vcls)

//Count leading zeros
NEON_GENERIC_ONE_ARG_S(vclz)
NEON_GENERIC_ONE_ARG_U(vclz)

//Reciprocal estimate
NEON_GENERIC_ONE_ARG_F(vrecpe)

//Reciprocal square-root estimate
NEON_GENERIC_ONE_ARG_F(vrsqrte)

//Arithmetic
NEON_GENERIC_TWO_ARGS_INT(vadd)
NEON_GENERIC_TWO_ARGS_INT_LONG(vaddl)
NEON_GENERIC_TWO_ARGS_INT(vqadd)
NEON_GENERIC_TWO_ARGS_INT(vmul)
NEON_GENERIC_TWO_ARGS_INT(vsub)
NEON_GENERIC_TWO_ARGS_INT_LONG(vsubl)
inline uint32x4x2_t vsubl( uint16x8_t a, uint16x8_t b )
{
    uint32x4x2_t out;
    out.val[0] = vsubl_u16( vget_low_u16( a ), vget_low_u16( b ) );
    out.val[1] = vsubl_u16( vget_high_u16( a ), vget_high_u16( b ) );
    return out;
}
inline int32x4x2_t vsubl( int16x8_t a, int16x8_t b )
{
    int32x4x2_t out;
    out.val[0] = vsubl_s16( vget_low_s16( a ), vget_low_s16( b ) );
    out.val[1] = vsubl_s16( vget_high_s16( a ), vget_high_s16( b ) );
    return out;
}
NEON_GENERIC_TWO_ARGS_INT(vqsub)
NEON_GENERIC_TWO_ARGS_FLOAT(vadd)
NEON_GENERIC_TWO_ARGS_FLOAT(vmul)
NEON_GENERIC_TWO_ARGS_FLOAT(vsub)
NEON_GENERIC_TWO_ARGS_64(vadd)

//vmull
NEON_GENERIC_TWO_ARGS_8x8(vmull, uint16x8_t, uint8x8_t, uint8x8_t, u8) \
NEON_GENERIC_TWO_ARGS_8x8(vmull, int16x8_t, int8x8_t, int8x8_t, s8) \
NEON_GENERIC_TWO_ARGS_16x4(vmull, uint32x4_t, uint16x4_t, uint16x4_t, u16) \
NEON_GENERIC_TWO_ARGS_16x4(vmull, int32x4_t, int16x4_t, int16x4_t, s16) \

NEON_GENERIC_TWO_ARGS_32x2(vmull, uint64x2_t, uint32x2_t, uint32x2_t, u32) \
NEON_GENERIC_TWO_ARGS_32x2(vmull, int64x2_t, int32x2_t, int32x2_t, s32) \

//comparison
NEON_GENERIC_TWO_ARGS_CMP(vceq)
NEON_GENERIC_TWO_ARGS_CMP(vcge)
NEON_GENERIC_TWO_ARGS_CMP(vcle)
NEON_GENERIC_TWO_ARGS_CMP(vcgt)
NEON_GENERIC_TWO_ARGS_CMP(vclt)

//max
NEON_GENERIC_TWO_ARGS_INT(vmax)
NEON_GENERIC_TWO_ARGS_INT(vmin)
NEON_GENERIC_TWO_ARGS_FLOAT(vmax)
NEON_GENERIC_TWO_ARGS_FLOAT(vmin)

//Reciprocal step
NEON_GENERIC_TWO_ARGS_FLOAT(vrecps)
NEON_GENERIC_TWO_ARGS_FLOAT(vrsqrts)

//Logical operations (AND)
NEON_GENERIC_TWO_ARGS_INT(vand)
inline uint32x4x2_t vand( uint32x4x2_t a, uint8x8_t b )
{
    int16x8_t qb = vmovl_s8( vreinterpret_s8_u8( b ) );
    uint32x4x2_t qb2;
    qb2.val[0] = vreinterpretq_u32_s32( vmovl_s16( vget_low_s16( qb ) ) );
    qb2.val[1] = vreinterpretq_u32_s32( vmovl_s16( vget_high_s16( qb ) ) );
    return vand( a, qb2 );
}
inline uint16x8_t vand( uint16x8_t a, uint8x8_t b )
{
    int16x8_t qb = vmovl_s8( vreinterpret_s8_u8( b ) );
    return vand( a, vreinterpretq_u16_s16( qb ) );
}


//Logical operations (OR)
NEON_GENERIC_TWO_ARGS_INT(vorr)

//Logical operations (exclusive OR)
NEON_GENERIC_TWO_ARGS_INT(veor)

//Logical operations (AND-NOT)
NEON_GENERIC_TWO_ARGS_INT(vbic)

//Logical operations (OR-NOT)
NEON_GENERIC_TWO_ARGS_INT(vorn)

//Pairwise add
NEON_GENERIC_TWO_ARGS_8x8(vpadd, uint8x8_t, uint8x8_t, uint8x8_t, u8)
NEON_GENERIC_TWO_ARGS_8x8(vpadd, int8x8_t, int8x8_t, int8x8_t, s8)
NEON_GENERIC_TWO_ARGS_16x4(vpadd, uint16x4_t, uint16x4_t, uint16x4_t, u16)
NEON_GENERIC_TWO_ARGS_16x4(vpadd, int16x4_t, int16x4_t, int16x4_t, s16)
NEON_GENERIC_TWO_ARGS_32x2(vpadd, uint32x2_t, uint32x2_t, uint32x2_t, u32)
NEON_GENERIC_TWO_ARGS_32x2(vpadd, int32x2_t, int32x2_t, int32x2_t, s32)
NEON_GENERIC_TWO_ARGS_32x2(vpadd, float32x2_t, float32x2_t, float32x2_t, f32)

NEON_GENERIC_ONE_ARG_8x8(vpaddl, int16x4_t, int8x8_t, s8)
NEON_GENERIC_ONE_ARG_8x8(vpaddl, uint16x4_t, uint8x8_t, u8)
NEON_GENERIC_ONE_ARG_16x4(vpaddl, int32x2_t, int16x4_t, s16)
NEON_GENERIC_ONE_ARG_16x4(vpaddl, uint32x2_t, uint16x4_t, u16)
NEON_GENERIC_ONE_ARG_16x8(vpaddl, int32x4_t, int16x8_t, s16)
NEON_GENERIC_ONE_ARG_16x8(vpaddl, uint32x4_t, uint16x8_t, u16)
NEON_GENERIC_ONE_ARG_32x4(vpaddl, int64x2_t, int32x4_t, s32)
NEON_GENERIC_ONE_ARG_32x4(vpaddl, uint64x2_t, uint32x4_t, u32)



//Set lane

//vdupq_n_s32
inline int32x4x2_t vset(int32_t a){

	int32x4x2_t out;
    out.val[0] = vdupq_n_s32(a);
    out.val[1] = vdupq_n_s32(a);
	return out;
}

//Get lane
NEON_GENERIC_TWO_ARGS_8x8_TEMPLATE(vget, uint8_t, uint8x8_t, lane_u8)
NEON_GENERIC_TWO_ARGS_8x8_TEMPLATE(vget, int8_t, int8x8_t, lane_s8)
NEON_GENERIC_TWO_ARGS_16x4_TEMPLATE(vget, uint16_t, uint16x4_t, lane_u16)
NEON_GENERIC_TWO_ARGS_16x4_TEMPLATE(vget, int16_t, int16x4_t, lane_s16)
NEON_GENERIC_TWO_ARGS_16x8_TEMPLATE(vget, uint16_t, uint16x8_t, lane_u16)
NEON_GENERIC_TWO_ARGS_16x8_TEMPLATE(vget, int16_t, int16x8_t, lane_s16)
NEON_GENERIC_TWO_ARGS_32x2_TEMPLATE(vget, uint32_t, uint32x2_t, lane_u32)
NEON_GENERIC_TWO_ARGS_32x2_TEMPLATE(vget, int32_t, int32x2_t, lane_s32)
NEON_GENERIC_TWO_ARGS_32x4_TEMPLATE(vget, uint32_t, uint32x4_t, lane_u32)
NEON_GENERIC_TWO_ARGS_32x4_TEMPLATE(vget, int32_t, int32x4_t, lane_s32)
NEON_GENERIC_TWO_ARGS_32x4_TEMPLATE(vget, float32_t, float32x4_t, lane_f32)
NEON_GENERIC_TWO_ARGS_32x2_TEMPLATE(vget, float32_t, float32x2_t, lane_f32)
NEON_GENERIC_TWO_ARGS_64x2_TEMPLATE(vget, uint64_t, uint64x2_t, lane_u64)
NEON_GENERIC_TWO_ARGS_64x2_TEMPLATE(vget, int64_t, int64x2_t, lane_s64)

NEON_GENERIC_VBSL(vbsl)

inline uint32x4x2_t vbsl( uint8x8_t a, uint32x4x2_t b, uint32x4x2_t c )
{
    int16x8_t qa = vmovl_s8( vreinterpret_s8_u8( a ) );
    uint32x4x2_t qa2;
    qa2.val[0] = vreinterpretq_u32_s32( vmovl_s16( vget_low_s16( qa ) ) );
    qa2.val[1] = vreinterpretq_u32_s32( vmovl_s16( vget_high_s16( qa ) ) );
    return vbsl( qa2, b, c );
}

inline uint32x4x2_t vbsl( uint16x8_t a, uint32x4x2_t b, uint32x4x2_t c )
{
    int16x8_t qa = vreinterpretq_s16_u16( a );
    uint32x4x2_t qa2;
    qa2.val[0] = vreinterpretq_u32_s32( vmovl_s16( vget_low_s16( qa ) ) );
    qa2.val[1] = vreinterpretq_u32_s32( vmovl_s16( vget_high_s16( qa ) ) );
    return vbsl( qa2, b, c );
}

//vshl
NEON_GENERIC_TWO_ARGS_8x8(vshl, int8x8_t, int8x8_t, int8x8_t, s8);
NEON_GENERIC_TWO_ARGS_8x8(vshl, uint8x8_t, uint8x8_t, int8x8_t, u8); 
NEON_GENERIC_TWO_ARGS_16x4(vshl, int16x4_t, int16x4_t, int16x4_t, s16);
NEON_GENERIC_TWO_ARGS_16x4(vshl, uint16x4_t, uint16x4_t, int16x4_t, u16);
NEON_GENERIC_TWO_ARGS_32x2(vshl, int32x2_t, int32x2_t, int32x2_t, s32);
NEON_GENERIC_TWO_ARGS_32x2(vshl, uint32x2_t, uint32x2_t, int32x2_t, u32);
NEON_GENERIC_TWO_ARGS_8x16(vshl, int8x16_t, int8x16_t, int8x16_t, s8);
NEON_GENERIC_TWO_ARGS_8x16(vshl, uint8x16_t, uint8x16_t, int8x16_t, u8);
NEON_GENERIC_TWO_ARGS_16x8(vshl, int16x8_t, int16x8_t, int16x8_t, s16);
NEON_GENERIC_TWO_ARGS_16x8(vshl, uint16x8_t, uint16x8_t, int16x8_t, u16);
NEON_GENERIC_TWO_ARGS_32x4(vshl, int32x4_t, int32x4_t, int32x4_t, s32);
NEON_GENERIC_TWO_ARGS_32x4(vshl, uint32x4_t, uint32x4_t, int32x4_t, u32);
NEON_GENERIC_TWO_ARGS_64x2(vshl, int64x2_t, int64x2_t, int64x2_t, s64);
NEON_GENERIC_TWO_ARGS_64x2(vshl, uint64x2_t, uint64x2_t, int64x2_t, u64);
//NEON_GENERIC_TWO_ARGS_32x8(vshl, uint32x4x2_t, uint32x4x2_t, int32x4_t, u32);
//NEON_GENERIC_TWO_ARGS_32x8(vshl, int32x4x2_t, int32x4x2_t, int, s32);

inline int32x4x2_t vshl(int32x4x2_t a, int32x4x2_t b){
	int32x4x2_t out;
    out.val[0] = vshlq_s32(a.val[0], b.val[0]);
    out.val[1] = vshlq_s32(a.val[1], b.val[1]);
	return out;
}

inline uint32x4x2_t vshl(uint32x4x2_t a, int32x4x2_t b){
	uint32x4x2_t out;
    out.val[0] = vshlq_u32(a.val[0], b.val[0]);
    out.val[1] = vshlq_u32(a.val[1], b.val[1]);
	return out;
}

NEON_GENERIC_TWO_ARGS_8x8_TEMPLATE(vshr, int8x8_t, int8x8_t, n_s8);
NEON_GENERIC_TWO_ARGS_8x8_TEMPLATE(vshr, uint8x8_t, uint8x8_t, n_u8); 
NEON_GENERIC_TWO_ARGS_16x4_TEMPLATE(vshr, int16x4_t, int16x4_t, n_s16);
NEON_GENERIC_TWO_ARGS_16x4_TEMPLATE(vshr, uint16x4_t, uint16x4_t, n_u16);
NEON_GENERIC_TWO_ARGS_32x2_TEMPLATE(vshr, int32x2_t, int32x2_t, n_s32);
NEON_GENERIC_TWO_ARGS_32x2_TEMPLATE(vshr, uint32x2_t, uint32x2_t, n_u32);
NEON_GENERIC_TWO_ARGS_8x16_TEMPLATE(vshr, int8x16_t, int8x16_t, n_s8);
NEON_GENERIC_TWO_ARGS_8x16_TEMPLATE(vshr, uint8x16_t, uint8x16_t, n_u8);
NEON_GENERIC_TWO_ARGS_16x8_TEMPLATE(vshr, int16x8_t, int16x8_t, n_s16);
NEON_GENERIC_TWO_ARGS_16x8_TEMPLATE(vshr, uint16x8_t, uint16x8_t, n_u16);
NEON_GENERIC_TWO_ARGS_32x4_TEMPLATE(vshr, int32x4_t, int32x4_t, n_s32);
NEON_GENERIC_TWO_ARGS_32x4_TEMPLATE(vshr, uint32x4_t, uint32x4_t, n_u32);
NEON_GENERIC_TWO_ARGS_64x2_TEMPLATE(vshr, int64x2_t, int64x2_t, n_s64);
NEON_GENERIC_TWO_ARGS_64x2_TEMPLATE(vshr, uint64x2_t, uint64x2_t, n_u64);
//NEON_GENERIC_TWO_ARGS_32x8(vshr, uint32x4x2_t, uint32x4x2_t, int, n_u32);
//NEON_GENERIC_TWO_ARGS_32x8(vshr, int32x4x2_t, int32x4x2_t, int, n_s32);


template<int b>
inline int32x4x2_t vshr(int32x4x2_t a){
    int32x4x2_t out;
    out.val[0] = vshrq_n_s32(a.val[0], b);
    out.val[1] = vshrq_n_s32(a.val[1], b);
    return out;
}

template<int b>
inline uint32x4x2_t vshr(uint32x4x2_t a){
    uint32x4x2_t out;
    out.val[0] = vshrq_n_u32(a.val[0], b);
    out.val[1] = vshrq_n_u32(a.val[1], b);
    return out;
}

inline void vzip( int16x8_t &a, int16x8_t &b )
{
    int16x8x2_t c;
    c = vzipq_s16( a, b );
    a = c.val[0];
    b = c.val[1];
}

inline void vzip( int32x4x2_t &a, int32x4x2_t &b )
{
    int32x4_t t = a.val[1];
    a = vzipq_s32( a.val[0], b.val[0] );
    b = vzipq_s32( t, b.val[1] );
}


//simulate template typedef
template<typename serialType>
struct neon{
};

//Aliases for vector with uchar elements, e.g. neon<uchar>::size8 is uint8x8_t
template<>
struct neon<uchar>{
    typedef uint8x8_t size8; //vector of uchar type with 8 elements
    typedef uint8x16_t size16; //vector of uchar type with 16 elements
    typedef neon<uchar> u; //related struct for unsigned type (in this case reference to self)
    typedef neon<schar> s; //related struct for unsigned type (in this case reference to self)
    typedef neon<ushort> d; //related struct for double size
    typedef uint8x8_t size_d; //vector of double word width
    typedef uint8x16_t size_q; //vector of quad word width
    static const int count_d = 8; //number of elements in double word vector for uchar type
    static const int count_q = 16; //number of elements in quad word vector for uchar type
};
//Aliases for vector with schar elements
template<>
struct neon<schar>{
    typedef int8x8_t size8;
    typedef int8x16_t size16;
    typedef neon<uchar> u;
    typedef neon<schar> s;
    typedef neon<short> d;
    typedef int8x8_t size_d;
    typedef int8x16_t size_q;
    static const int count_d = 8;
    static const int count_q = 16;
};
//Aliases for vector with ushort elements
template<>
struct neon<ushort>{
    typedef uint16x4_t size4;
    typedef uint16x4x2_t size4x2;
    typedef uint16x8_t size8;
    typedef neon<ushort> u;
    typedef neon<short> s;
    typedef neon<uint> d;
    typedef uint16x4_t size_d;
    typedef uint16x8_t size_q;
    static const int count_d = 4;
    static const int count_q = 8;
};
//Aliases for vector with short elements
template<>
struct neon<short>{
    typedef int16x4_t size4;
    typedef int16x8_t size8;
    typedef neon<ushort> u;
    typedef neon<short> s;
    typedef neon<int> d;
    typedef int16x4_t size_d;
    typedef int16x8_t size_q;
    static const int count_d = 4;
    static const int count_q = 8;
};

//Aliases for vector with uint elements
template<>
struct neon<uint>{
    typedef uint32x2_t size2;
    typedef uint32x4_t size4;
    typedef uint32x4x2_t size8;
    typedef neon<uint> u;
    typedef neon<int> s;
    typedef neon<uint64_t> d;
    typedef uint32x2_t size_d;
    typedef uint32x4_t size_q;
    static const int count_d = 2;
    static const int count_q = 4;
};

//Aliases for vector with int elements
template<>
struct neon<int>{
    typedef int32x2_t size2;
    typedef int32x4_t size4;
    typedef int32x4x2_t size8;
    typedef neon<uint> u;
    typedef neon<int> s;
    typedef neon<int64_t> d;
    typedef int32x2_t size_d;
    typedef int32x4_t size_q;
    static const int count_d = 2;
    static const int count_q = 4;
};

//Aliases for vector with float elements
template<>
struct neon<float>{
    typedef float32x2_t size2;
    typedef float32x4_t size4;
    typedef float32x4x2_t size8;
    typedef neon<uint> u;
    typedef neon<int> s;
    typedef float32x2_t size_d;
    typedef float32x4_t size_q;
    static const int count_d = 2;
    static const int count_q = 4;
};
//Aliases for vector with unsigned long long int elements
template<>
struct neon<uint64_t>{
    typedef uint64x1_t size1;
    typedef uint64x2_t size2;
    typedef uint64x2x2_t size4;
    typedef neon<uint64_t> u;
    typedef neon<int64_t> s;
    typedef uint64x1_t size_d;
    typedef uint64x2_t size_q;
    static const int count_d = 1;
    static const int count_q = 2;
};
//Aliases for vector with long long int elements
template<>
struct neon<int64_t>{
    typedef int64x1_t size1;
    typedef int64x2_t size2;
    typedef int64x2x2_t size4;
    typedef neon<uint64_t> u;
    typedef neon<int64_t> s;
    typedef int64x1_t size_d;
    typedef int64x2_t size_q;
    static const int count_d = 1;
    static const int count_q = 2;
};

} //geneon

#endif //CV_NEON

#endif //_OPENCV_NEON_GENERIC_HPP
//SRP0L 2013
