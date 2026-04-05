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
 
#ifndef _OPENCV_NEON_FUNCTIONS_HPP_
#define _OPENCV_NEON_FUNCTIONS_HPP_


/* Klocwork: the neon registers data type is not properly
 *           recognized during the analyze.All the functions
 *           which returns value are marked as implicit int, 
 *           what should be treated as false positive.
 */

#include <limits.h>
#include <map>

#if defined(GNUC)
#define __pld(x) __builtin_prefetch((x), 0, 0)
#endif

static inline bool checkCpuHasNeonFeatures()
{
	return true;
}

extern bool CPU_HAS_NEON_FEATURE;

#if defined _NEON 
#   include <arm_neon.h>
#   define __ARM_NEON__ 1
#   define CV_NEON 1
#   if defined (__aarch64__)
#       define CV_NEON64 1
#   else
#       define CV_NEON64 0
#   endif
#endif

//#if defined __aarch64__ && defined _NEON
//	#define __ARM_NEON__ 1
//	#  include <arm_neon.h>
//	#  define CV_NEON 1
//#elif defined __ARM_NEON__ && defined _NEON
//	#include <arm_neon.h> 
//	#define CV_NEON 1
//#endif
//


#include "neon_generic.hpp"

/**
 * The following NEON-based functions are implementation of:
 * - equivalents for SSE functions
 * - other auxiliary functions
 */

#if CV_NEON

//constant masks needed by vmovemaskq_u8
const uint8x16_t vmovemaskq_u8_mask_and = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
const int8x16_t vmovemaskq_u8_mask_shift = { -7, -6, -5, -4, -3, -2, -1, 0, -7, -6, -5, -4, -3, -2, -1, 0};
/**
 * Implements the _mm_movemask_epi8 SSE2 intrinsic for a quadword register
 *
 * @param input a uint8x16_t NEON register:
 *    (a15,a14,a3,..., a1,a0)
 *
 *
 * @return an integer number obtained by the MSB of each element as follows:
 *         out = a15[7] << 15 |
 *               a14[7] << 14 |
 *               a13[7] << 13 |
 *               a12[7] << 12 |
 *               ...
 *               a1[7]  <<  1 |
 *               a0[7]
 */
int static inline vmovemaskq_u8(uint8x16_t input)
{
    input = vandq_u8(input, vmovemaskq_u8_mask_and);
    input = vshlq_u8(input, vmovemaskq_u8_mask_shift);

    const uint16x8_t input16 = vpaddlq_u8(input);
    const uint32x4_t input32 = vpaddlq_u16(input16);
    const uint64x2_t input64 = vpaddlq_u32(input32);

    return (vgetq_lane_u16(vreinterpretq_u16_u64(input64),4) << 8) + (vgetq_lane_u16(vreinterpretq_u16_u64(input64),0));
}

/**
 * See vmovemaskq_u8. It's the same function, but operates on signed
 * integers 16 bits wide.
 *
 * @param input
 *
 * @return
 */
int static inline vmovemaskq_s8(int8x16_t input)
{
    return vmovemaskq_u8(vreinterpretq_u8_s8(input));
}

/**
 * Unpack the high 64bits of two Quad word register (interpreted as a
 * uint16x8_t) into a new quad word register (SSE2 _mm_unpackhi_epi64)

 *
 * @param to_low the Q register who's 64 high bits are going to the
 * low part of the output {to_low[127..64], to_low[63..0]}
 * @param to_high the Q register who's 64 high bits are going to the
 * high part of the output {to_high[127..64], to_high[63..0]}
 *
 * @return {to_high[127..64], to_low[127..64]}
 */
uint16x8_t static inline vunpackhi64q_u16(uint16x8_t to_low, uint16x8_t to_high)
{
    return vcombine_u16(vget_high_u16(to_low), vget_high_u16(to_high));
}

/**
 * See vunpackhi64q_u16. It's the same function, but operates on signed
 * integers 16 bits wide.
 *
 * @param to_low
 * @param to_high
 *
 * @return
 */
int16x8_t static inline vunpackhi64q_s16(int16x8_t to_low, int16x8_t to_high)
{
    return vcombine_s16(vget_high_s16(to_low), vget_high_s16(to_high));
}

/**
 * Implements the SSE: _mm_packus_epi16 (__m128i to_low, __m128i to_high)
 * Pack the 16bits signed integers in  to_low and to_high into 8 bits
 * unsigned integer and saturates.
 *
 * @param to_low  (L7,L6,...,L1,L0)
 * @param to_high (H7,H6,...,H1,H0)
 *
 * @return (PackAndSaturate(H7),   -----------
 *          PackAndSaturate(H6),        |
 *          ...                        high
 *          PackAndSaturate(H1),        |
 *          PackAndSaturate(H0),   -----------
 *          PackAndSaturate(L7),   -----------
 *          PackAndSaturate(L6),        |
 *          ...,                       low
 *          PackAndSaturate(L1),        |
 *          PackAndSaturate(L0))   ------------
*/
uint8x16_t static inline vqpackq_u8_s16(int16x8_t to_low, int16x8_t to_high)
{
    return vcombine_u8(vqmovun_s16(to_low),  vqmovun_s16(to_high));
}

/**
 * Implements the SSE: _mm_packs_epi32 (__m128i to_low, __m128i to_high)
 * Pack the 32bits signed integers in  to_low and to_high into 16bits
 * signed integer and saturates.
 *
 * @param to_low  (L3,L2,L1,L0)
 * @param to_high (H3,H2,H1,H0)
 *
 * @return (PackAndSignedSaturate(H3),   -----------
 *          PackAndSignedSaturate(H2),        |
 *          PackAndSignedSaturate(H1),        |  high
 *          PackAndSignedSaturate(H0),   -----------
 *          PackAndSignedSaturate(L3),   -----------
 *          PackAndSignedSaturate(L2),        |
 *          PackAndSignedSaturate(L1),        |  low
 *          PackAndSignedSaturate(L0))   ------------
*/
int16x8_t static inline vqpackq_s16_s32(int32x4_t to_low, int32x4_t to_high)
{
    return vcombine_s16(vqmovn_s32(to_low),  vqmovn_s32(to_high));
}

/**
 * NEON implementationof the SSE intruction '_mm_madd_epi16', function
 * which multiplies the 8 signed 16-bit integers from a by the 8
 * signed 16-bit integers from b and adds the signed 32-bit integer
 * results pairwise. Than packs the 4 signed 32-bit integer results.
 *
 * @param a  (a7,a6,a5,a4,a3,a2,a1,a0) of NEON type int16x8_t
 * @param b  (b7,b6,b5,b4,b3,b2,b1,b0) of NEON type int16x8_t
 *
 * @return   c = (c3,c2,c1,c0) of NEON type int32x4_t compued as:
 *
 *           c0 := (a0 * b0) + (a1 * b1)
 *           c1 := (a2 * b2) + (a3 * b3)
 *           c2 := (a4 * b4) + (a5 * b5)
 *           c3 := (a6 * b6) + (a7 * b7)
 */
int32x4_t static inline vmadd_packq_s32_s16(int16x8_t a, int16x8_t b)
{
    int32x4_t tmp1 = vmull_s16(vget_low_s16(a), vget_low_s16(b));
    int32x4_t tmp2 = vmull_s16(vget_high_s16(a), vget_high_s16(b));
    int32x4_t t0 = vcombine_s32(vpadd_s32(vget_low_s32(tmp1), vget_high_s32(tmp1)), vpadd_s32(vget_low_s32(tmp2), vget_high_s32(tmp2)));
    return t0;
}

//Inverse square root
float32x4_t inline vrsqrtq_f32(float32x4_t x){

    float32x4_t result;

    //estimate for 1/sqrt(input)
    result = vrsqrteq_f32( x );

    //Newton-Raphson iteration 1
    result = vmulq_f32(result,(vrsqrtsq_f32(x, vmulq_f32(result,result))));

    //Newton-Raphson iteration 2
    result = vmulq_f32(result,(vrsqrtsq_f32(x, vmulq_f32(result,result))));

    return result;
}


//Reciprocal
float32x4_t inline vrecpq_f32(float32x4_t b){
    float32x4_t result = vrecpeq_f32 (b);

    result = vmulq_f32 (vrecpsq_f32 (b, result), result);
    result = vmulq_f32 (vrecpsq_f32 (b, result), result);

    return result;
}

/**
 * Rounding to integral to the nearest value with tieing away from zero.
 * What does it mean? round(11.5) = 12, round(-11.5) = 12, round(11.2) = 11
 * round(11.7) = 12
 *
 * Note: it is quite similar to vrndqa_f32 which in our GCC is currently unimplemented.
 * Yet there is one difference - the result of vrndaq_s32_f32 is already casted to int32x4_t
 * Note2: currently available rounding (by e.g. vcvtq_s32_f32 is rounding towards zero
 *
 * @param input - register with floats to be rounded
 *
 * @return rounded numbers
 *
 */
int32x4_t static inline vrndaq_s32_f32(float32x4_t input) {
#if CV_NEON64
   int32x4_t ret;
   asm
   (
       "fcvtas %[ret].4s, %[input].4s\n\t"
       : [ret] "=w"(ret)
       : [input] "w"(input)
       :
   );

   return ret;
#else
    uint32x4_t _mask = vdupq_n_u32(0x80000000);
    float32x4_t _coeff = vdupq_n_f32(0.5f);
    uint32x4_t _rcoeff = vreinterpretq_u32_f32(_coeff);
    uint32x4_t _input = vreinterpretq_u32_f32(input);
    _mask = vandq_u32(_input, _mask);
    _rcoeff = vorrq_u32(_rcoeff, _mask);

    return vcvtq_s32_f32(vaddq_f32(vreinterpretq_f32_u32(_rcoeff), input));
#endif
}

/**
 * Rounding to nearest integer tying to even. (IEEE-754 default rounding)
 * What does it mean? round(11.5) = 12, round(12.5) = 12, round(10.5) = 10
 * round(-11.5) = -12
 *
 * Note: it is quite similar to vrndqn_f32 which in our GCC is currently unimplemented.
 * Yet there is one difference - the result of vrndnq_s32_f32 is already casted to int32x4_t
 * Note2: used floating point constant (0x1p23) has accuracy of 1.0, i.e. the same
 * as integer types. Thus, adding to it rounds NEON floats to integer precision
 * with default rounding (i.e. to nearest with ties to even) as always with floats addition.
 *
 * @param input - register with floats to be rounded
 *
 * @return rounded numbers
 *
 */
int32x4_t static inline vrndnq_s32_f32(float32x4_t input)
{
#if CV_NEON64
    return vcvtnq_s32_f32(input);
#else
    static const float32x4_t x1p23 = vmovq_n_f32(0x1p23);
    float32x4_t s0x1p23 = vbslq_f32(vcgeq_f32(input, vmovq_n_f32(0))
                , x1p23, vnegq_f32(x1p23));
    return vcvtq_s32_f32(vsubq_f32(vaddq_f32(s0x1p23,input),s0x1p23));
#endif
}

/**
 * Interleaves the lower 8 unsigned 8-bit integers in a with the lower 8 unsigned 8-bit integers in b.
 * r0 := a0 ; r1 := b0
 * r2 := a1 ; r3 := b1
 * ...
 * r14 := a7 ; r15 := b7
 *
 * SSE equivalent: _mm_unpacklo_epi8
 *
 * @param a - the Q register who's 64 lower bits (eight 8-bit values) are going to be interleaved with
 * 64 lower bits of register b
 * @param b - the Q register who's 64 lower bits (eight 8-bit values) are going to be interleaved with
 * 64 lower bits of register a
 *
 *         {high bits                       low bits}
 * @return {b[63..56], a[63..56], .. , b[15..7], a[15..8], b[7..0], a[7..0]}
 */
uint8x16_t inline static vunpackloq_u8(uint8x16_t a, uint8x16_t b)
{
    uint8x8x2_t zipped = vzip_u8(vget_low_u8(a), vget_low_u8(b));
    return vcombine_u8(zipped.val[0], zipped.val[1]);
}

/**
 * Interleaves the lower 4 unsigned 16-bit integers in a with the lower 4 unsigned 16-bit integers in b.
 * r0 := a0 ; r1 := b0
 * r2 := a1 ; r3 := b1
 * r4 := a2 ; r5 := b2
 * r6 := a3 ; r7 := b3
 *
 * SSE equivalent: _mm_unpacklo_epi16
 *
 * @param a - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register b
 * @param b - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register a
 *
 *         {high bits                       low bits}
 * @return {b[63..48], a[63..48], b[47..32], a[47..32], b[31..16], a[31..16], b[15..0], a[15..0]}
 */
uint16x8_t inline static vunpackloq_u16(uint16x8_t a, uint16x8_t b)
{
    uint16x4x2_t zipped = vzip_u16(vget_low_u16(a), vget_low_u16(b));
    return vcombine_u16(zipped.val[0], zipped.val[1]);
}

/**
 * Interleaves the lower 2 unsigned 32-bit integers in a with the lower 2 unsigned 32-bit integers in b.
 * r0 := a0 ; r1 := b0
 * r2 := a1 ; r3 := b1
 *
 * SSE equivalent: _mm_unpacklo_epi32
 * Note: This function operates on unsigned integers -> compare to vunpackloq_s32
 *
 * @param a - the Q register who's 64 lower bits (two 32-bit values) are going to be interleaved with
 * 64 lower bits of register b
 * @param b - the Q register who's 64 lower bits (two 32-bit values) are going to be interleaved with
 * 64 lower bits of register a
 *
 *         {high bits                       low bits}
 * @return {b[63..32], a[63..32], b[31..0], a[31..0]}
 */
uint32x4_t inline static vunpackloq_u32(uint32x4_t a, uint32x4_t b)
{
    uint32x2x2_t zipped = vzip_u32(vget_low_u32(a), vget_low_u32(b));
    return vcombine_u32(zipped.val[0], zipped.val[1]);
}

/**
 * Interleaves the lower 2 signed 32-bit integers in a with the lower 2 signed 32-bit integers in b.
 * r0 := a0 ; r1 := b0
 * r2 := a1 ; r3 := b1
 *
 * SSE equivalent: _mm_unpacklo_epi32
 * Note: This function operates on signed integers -> compare to vunpackloq_u32
 *
 * @param a - the Q register who's 64 lower bits (two 32-bit values) are going to be interleaved with
 * 64 lower bits of register b
 * @param b - the Q register who's 64 lower bits (two 32-bit values) are going to be interleaved with
 * 64 lower bits of register a
 *
 *         {high bits                       low bits}
 * @return {b[63..32], a[63..32], b[31..0], a[31..0]}
 */
int32x4_t inline static vunpackloq_s32(int32x4_t a, int32x4_t b)
{
    int32x2x2_t zipped = vzip_s32(vget_low_s32(a), vget_low_s32(b));
    return vcombine_s32(zipped.val[0], zipped.val[1]);
}

/**
 * Interleaves the lower 4 signed 16-bit integers in a with the lower 4 signed 16-bit integers in b.
 * r0 := a0 ; r1 := b0
 * r2 := a1 ; r3 := b1
 * r4 := a2 ; r5 := b2
 * r6 := a3 ; r7 := b3
 *
 * SSE equivalent: _mm_unpacklo_epi16
 *
 * @param a - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register b
 * @param b - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register a
 *
 *         {high bits                       low bits}
 * @return {b[63..48], a[63..48], b[47..32], a[47..32], b[31..16], a[31..16], b[15..0], a[15..0]}
 */
int16x8_t inline static vunpackloq_s16(int16x8_t a, int16x8_t b)
{
    int16x4x2_t zipped = vzip_s16(vget_low_s16(a), vget_low_s16(b));
    return vcombine_s16(zipped.val[0], zipped.val[1]);
}

/**
 * Interleaves the higher 4 unsigned 16-bit integers in a with the higher 4 unsigned 16-bit integers in b.
 * r0 := a4 ; r1 := b4
 * r2 := a5 ; r3 := b5
 * r4 := a6 ; r5 := b6
 * r6 := a7 ; r7 := b7
 *
 * SSE equivalent: _mm_unpackhi_epi16
 *
 * @param a - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register b
 * @param b - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register a
 *
 *         {high bits                       low bits}
 * @return {b[63..48], a[63..48], b[47..32], a[47..32], b[31..16], a[31..16], b[15..0], a[15..0]}
 */
uint16x8_t inline static vunpackhiq_u16(uint16x8_t a, uint16x8_t b)
{
    uint16x4x2_t zipped = vzip_u16(vget_high_u16(a), vget_high_u16(b));
    return vcombine_u16(zipped.val[0], zipped.val[1]);
}

/**
 * Interleaves the higher 4 signed 16-bit integers in a with the higher 4 signed 16-bit integers in b.
 * r0 := a4 ; r1 := b4
 * r2 := a5 ; r3 := b5
 * r4 := a6 ; r5 := b6
 * r6 := a7 ; r7 := b7
 *
 * SSE equivalent: _mm_unpackhi_epi16
 *
 * @param a - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register b
 * @param b - the Q register who's 64 lower bits (four 16-bit values) are going to be interleaved with
 * 64 lower bits of register a
 *
 *         {high bits                       low bits}
 * @return {b[63..48], a[63..48], b[47..32], a[47..32], b[31..16], a[31..16], b[15..0], a[15..0]}
 */
int16x8_t inline static vunpackhiq_s16(int16x8_t a, int16x8_t b)
{
    int16x4x2_t zipped = vzip_s16(vget_high_s16(a), vget_high_s16(b));
    return vcombine_s16(zipped.val[0], zipped.val[1]);
}

/**
 * Moves signed 32-bit integer a to the least significant 32 bits of an __m128 object one extending the upper bits.
 * r0 := a
 * r1 := 0x0 ; r2 := 0x0 ; r3 := 0x0
 *
 * SSE equivalent: _mm_cvtsi32_si128
 * TODO: a better name for the function could be found
 *
 * @param a - the signed 32-bit value to be set to the lower 32 bits of output register
 *
 *         {high bits low bits}
 * @return {0x0, 0x0,   0x0, a}
 */
int32x4_t inline static vsetlo_s32(int32_t a)
{
    return vsetq_lane_s32(a, vdupq_n_s32(0), 0);
}

/**
 * Moves unsigned 32-bit integer a to the least significant 32 bits of an __m128 object one extending the upper bits.
 * r0 := a
 * r1 := 0x0 ; r2 := 0x0 ; r3 := 0x0
 *
 * SSE equivalent: _mm_cvtsi32_si128
 * TODO: a better name for the function could be found
 *
 * @param a - the unsigned 32-bit value to be set to the lower 32 bits of output register
 *
 *         {high bits low bits}
 * @return {0x0, 0x0,   0x0, a}
 */
uint32x4_t inline static vsetlo_u32(uint32_t a)
{
    return vsetq_lane_u32(a, vdupq_n_u32(0), 0);
}

/**
 * Unpack the low 64bits of two Quad word register (interpreted as a
 * uint32x4_t) into a new quad word register.
 * r0 := a0 ; r1 := b0
 *
 * SSE equivalent: _mm_unpacklo_epi64
 *
 * @param to_low the Q register who's 64 low bits are going to the
 * low part of the output {to_low[127..64], to_low[63..0]}
 * @param to_high the Q register who's 64 high bits are going to the
 * high part of the output {to_high[127..64], to_high[63..0]}
 *
 *         {high bits            low bits}
 * @return {to_high[63..0], to_low[63..0]}
 */
uint32x4_t static inline vunpacklou64_u32(uint32x4_t to_low, uint32x4_t to_high) {
    return vcombine_u32(vget_low_u32(to_low), vget_low_u32(to_high));
}

/**
 * Unpack the high 64bits of two Quad word register (interpreted as a
 * uint32x4_t) into a new quad word register.
 * r0 := a0 ; r1 := b0
 *
 * SSE equivalent: _mm_unpacklo_epi64
 *
 * @param to_low the Q register who's 64 high bits are going to the
 * low part of the output {to_low[127..64], to_low[63..0]}
 * @param to_high the Q register who's 64 high bits are going to the
 * high part of the output {to_high[127..64], to_high[63..0]}
 *
 *         {high bits                low bits}
 * @return {to_high[127..64], to_low[127..64]}
 */
uint32x4_t static inline vunpackhiu64_u32(uint32x4_t to_low, uint32x4_t to_high) {
    return vcombine_u32(vget_high_u32(to_low), vget_high_u32(to_high));
}

/**
 * Multiplies the 8 signed 16-bit integers from a by the 8 signed 16-bit integers from b.
 *
 * SSE equivalent: _mm_mulhi_epi16
 *
 * @param a - (a7,a6,a5,a4,a3,a2,a1,a0) of NEON type int16x8_t
 * @param b - (b7,b6,b5,b4,b3,b2,b1,b0) of NEON type int16x8_t
 *
 * @return   c = (c7..c3,c2,c1,c0) of NEON type int16x8_t computed as:
 *
 *          c0 := (a0 * b0)[31:16]
 *          c1 := (a1 * b1)[31:16]
 *          ...
 *          c7 := (a7 * b7)[31:16]
 */
int16x8_t static inline vmulhiq_s16(int16x8_t a, int16x8_t b) {
    return vshrq_n_s16(vqdmulhq_s16(a, b), 1);
}

//preload data to cache 128 bytes ahead of addr
// 2016.03.02 by Guenoh Park
//#if defined(GNUC)
#define	pld(x) __builtin_prefetch((x), 0, 0)
//#endif
//void inline pld(const void *addr){
//	#ifndef __aarch64__
//		#ifdef __GNUC__
//		asm volatile("PLD [%0, #128]"::"r" (addr));
//		#endif
//	#endif
//}

#ifdef __GNUC__
/**
 * Reinterpret union template to cheat the compiler. It is used to load to NEON
 * register one data type as another in vsetq_f32 function.
 *
 * @param inT  input data type
 * @param ouT  output data type
 */
template <typename inT, typename outT>
union reinterpret{
    inT in;
    outT out;

    reinterpret(inT i){in = i;}
};
#endif

/**
 * Function to load to Neon register 4 unsigned int values. This function is used
 * in case of random data access ex. data[i + index[j]]. To minimize the number
 * of instructions required to load the data is used vmov Dx,r0,r1 operation and the
 * data is loaded separately for both doubleword registers of quadword.
 *
 * @param a  input parameter of unsigned int data type loaded to lane 0
 * @param b  input parameter of unsigned int data type loaded to lane 1
 * @param c  input parameter of unsigned int data type loaded to lane 2
 * @param d  input parameter of unsigned int data type loaded to lane 3
 *
 * @return res  quadword register which contains a,b,c,d values
 */
uint32x4_t inline vsetq_u32(const uint a, const uint b, const uint c, const uint d){

    uint32x4_t res;
#if CV_NEON64
    asm
    (
        "ins %0.s[0],%w[a]\n\t"
        "ins %0.s[1],%w[b]\n\t"
        "ins %0.s[2],%w[c]\n\t"
        "ins %0.s[3],%w[d]\n\t"
        : "=w"(res)
        : [a] "r"(a), [b] "r"(b), [c] "r"(c), [d] "r"(d)
        :
     );
#elif CV_NEON
    asm
    (
        "vmov %e0,%[a],%[b]\n\t"
        "vmov %f0,%[c],%[d]\n\t"
        : "=w"(res)
        : [a] "r"(a), [b] "r"(b), [c] "r"(c), [d] "r"(d)
        :
    );
#else
    res = vsetq_lane_u32(d, res, 3);
    res = vsetq_lane_u32(c, res, 2);
    res = vsetq_lane_u32(b, res, 1);
    res = vsetq_lane_u32(a, res, 0);
#endif
    return res;
}

/**
 * Function to load to Neon register 4 float values. This function is used
 * in case of random data access ex. data[i + index[j]]. To minimize the number
 * of instructions required to load the data is used vmov Dx,r0,r1 operation which
 * require integer data type. The input float variables are reinterpreted from
 * float to unsigned int using reinterpret union template. Data is loaded
 * separately for both doubleword registers of quadword.
 *
 * @param a  input parameter of float data type loaded to lane 0
 * @param b  input parameter of float data type loaded to lane 1
 * @param c  input parameter of float data type loaded to lane 2
 * @param d  input parameter of float data type loaded to lane 3
 *
 * @return res  quadword register which contains a,b,c,d values
 */
float32x4_t inline vsetq_f32(float a, float b, float c, float d){

#if CV_NEON64
    uint32x4_t res;

    asm
    (
        "ins %0.s[0],%w[a]\n\t"
        "ins %0.s[1],%w[b]\n\t"
        "ins %0.s[2],%w[c]\n\t"
        "ins %0.s[3],%w[d]\n\t"
        : "=w"(res)
        : [a] "r"(reinterpret<float,uint>(a).out),//Dirty-hack to load float data as integer, to improve the speed
          [b] "r"(reinterpret<float,uint>(b).out),
          [c] "r"(reinterpret<float,uint>(c).out),
          [d] "r"(reinterpret<float,uint>(d).out)
        :
    );

    return vreinterpretq_f32_u32(res);//Reinterpret data to float - !!!NO COST!!! This not generates any assembler instruction!!!
#elif CV_NEON
    uint32x4_t res;

    asm
    (
        "vmov %e0,%[a],%[b]\n\t"
        "vmov %f0,%[c],%[d]\n\t"
        : "=w"(res)
        : [a] "r"(reinterpret<float,uint>(a).out),//Dirty-hack to load float data as integer, to improve the speed
          [b] "r"(reinterpret<float,uint>(b).out),
          [c] "r"(reinterpret<float,uint>(c).out),
          [d] "r"(reinterpret<float,uint>(d).out)
        :
    );

    return vreinterpretq_f32_u32(res);//Reinterpret data to float - !!!NO COST!!! This not generates any assembler instruction!!!
#else
    float32x4_t res;
    res = vsetq_lane_u32(d, res, 3);
    res = vsetq_lane_u32(c, res, 2);
    res = vsetq_lane_u32(b, res, 1);
    res = vsetq_lane_u32(a, res, 0);

    return res;
#endif
}

/**
 * Function to load to Neon register 4 integer values. This function is used
 * in case of random data access ex. data[i + index[j]]. To minimize the number
 * of instructions required to load the data is used vmov Dx,r0,r1 operation which
 * require integer data type.
 *
 * @param a  input parameter of int data type loaded to lane 0
 * @param b  input parameter of int data type loaded to lane 1
 * @param c  input parameter of int data type loaded to lane 2
 * @param d  input parameter of int data type loaded to lane 3
 *
 * @return res  quadword register which contains a,b,c,d values
 */
int32x4_t inline vsetq_s32(const int a, const int b, const int c, const int d){

    int32x4_t res;
#if CV_NEON64
    asm
    (
        "ins %0.s[0],%w[a]\n\t"
        "ins %0.s[1],%w[b]\n\t"
        "ins %0.s[2],%w[c]\n\t"
        "ins %0.s[3],%w[d]\n\t"
        : "=w"(res)
        : [a] "r"(a), [b] "r"(b), [c] "r"(c), [d] "r"(d)
        :
    );
#elif CV_NEON
    asm
    (
        "vmov %e0,%[a],%[b]\n\t"
        "vmov %f0,%[c],%[d]\n\t"
        : "=w"(res)
        : [a] "r"(a), [b] "r"(b), [c] "r"(c), [d] "r"(d)
        :
    );
#else
    res = vsetq_lane_s32(d, res, 3);
    res = vsetq_lane_s32(c, res, 2);
    res = vsetq_lane_s32(b, res, 1);
    res = vsetq_lane_s32(a, res, 0);
#endif
    return res;
}

/**
 * Function perform parwise addition for the single quadword float register:
 * This function replace
 *
 *      vpadd_f32(vget_low_f32(float32x4_t), vget_high_f32(float32x4_t))
 *
 * operation which generates 3 asm instrunction, example for q0:
 *
 *      vorr d2,d0,d0
 *      vorr d3,d1,d1
 *      vpadd.f32 d4,d2,d3
 *
 * This implementation generates only one asm instrunction, example for q0:
 *
 *      vpadd.f32 d2,d0,d1
 *
 *  @param reg  quadword register to perform pairwise addition
 *
 *  @return res  pairwise addition result, example for q0: res[0] = d0[0] + d0[1], res[1] = d1[0] + d1[1]
 */
float32x2_t inline vpadd_f32(float32x4_t reg){
    float32x2_t res;
#if CV_NEON64
    asm
    (
        "ins v0.d[0], %[reg].d[0]\n\t"
        "ins v1.d[0], %[reg].d[1]\n\t"
        "faddp %[res].2s, v0.2s, v1.2s\n\t"
        : [res] "=w"(res)
        : [reg] "w"(reg)
        : "v0", "v1"
    );
#elif CV_NEON
    asm
    (
        "vpadd.f32 %P[res], %e1,%f1\n\t"
        : [res] "=w"(res)
        : "w"(reg)
        :
    );
#else
    res = vpadd_f32(vget_low_f32(reg), vget_high_f32(reg));
#endif
    return res;
}

/**
 * Function moves data from int8x8 doubleword register to int32x4x2
 * quadword register structure exetending the range of each vector element.
 * The asm version was provided to reduce number of operation caused by
 * vget_{low | high}_s16 operation which create additional vorr instruction.
 *
 * @param reg input doubleword register of type int8x8_t
 *
 * @return res output quadword structure of type int32x4x2_t
 *
 */
int32x4x2_t inline vmovl_s32_s8(int8x8_t reg){
    int32x4x2_t res;
    int16x8_t tmp = {0, 0, 0, 0, 0, 0, 0, 0};
#if CV_NEON64
    asm(
            "sshll	%[tmp].8h, %[reg].8b, #0	\n\t"
            "sshll	%[res0].4s, %[tmp].4h, #0 \n\t"
            "sshll2	%[res1].4s, %[tmp].8h, #0 \n\t"
            : [res0] "=w" (res.val[0]), [res1] "=w" (res.val[1])
            : [reg] "w" (reg), [tmp] "w" (tmp)
            :
       );
#elif CV_NEON
    asm(
            "vmovl.s8 %q[tmp], %P[reg]\n\t"
            "vmovl.s16 %q[res0], %e[tmp]\n\t"
            "vmovl.s16 %q[res1], %f[tmp]\n\t"
            : [res0] "=w" (res.val[0]), [res1] "=w" (res.val[1])
            : [reg] "w" (reg), [tmp] "w" (tmp)
            :
       );
#else
    tmp = vmovl_s8(reg);
    res.val[0] = vmovl_s16(vget_low_s16(tmp));
    res.val[1] = vmovl_s16(vget_high_s16(tmp));
#endif
    return res;
}

/**
 * Function moves data from uint8x8 doubleword register to int32x4x2
 * quadword register structure exetending the range of each vector element.
 * The asm version was provided to reduce number of operation caused by
 * vget_{low | high}_s16 operation which create additional vorr instruction.
 *
 * @param reg input doubleword register of type uint8x8_t
 *
 * @return res output quadword structure of type int32x4x2_t
 *
 */
int32x4x2_t inline vmovl_s32_u8(uint8x8_t reg){
    int32x4x2_t res;
    int16x8_t tmp = {0, 0, 0, 0, 0, 0, 0, 0};
#if CV_NEON64
    asm(
            "ushll	%[tmp].8h, %[reg].8b, #0\n\t"
            "sshll	%[res0].4s, %[tmp].4h, #0\n\t"
            "sshll2 %[res1].4s, %[tmp].8h, #0\n\t"
            : [res0] "=w" (res.val[0]), [res1] "=w" (res.val[1])
            : [reg] "w" (reg), [tmp] "w" (tmp)
            :
       );
#elif CV_NEON
    asm(
            "vmovl.u8 %q[tmp], %P[reg]\n\t"
            "vmovl.s16 %q[res0], %e[tmp]\n\t"
            "vmovl.s16 %q[res1], %f[tmp]\n\t"
            : [res0] "=w" (res.val[0]), [res1] "=w" (res.val[1])
            : [reg] "w" (reg), [tmp] "w" (tmp)
            :
       );
#else
    tmp = vreinterpretq_s16_u16(vmovl_u8(reg));
    res.val[0] = vmovl_s16(vget_low_s16(tmp));
    res.val[1] = vmovl_s16(vget_high_s16(tmp));
#endif
    return res;
}

/**
 * Function moves data from int16x8 quadword register to int32x4x2
 * quadword register structure exetending the range of each vector element.
 * The asm version was provided to reduce number of operation caused by
 * vget_{low | high}_s16 operation which create additional vorr instruction.
 *
 * @param reg input quadword register of type int16x8_t
 *
 * @return res output quadword structure of type int32x4x2_t
 *
 */
int32x4x2_t inline vmovl_s32_s16(int16x8_t reg){
    int32x4x2_t res;
#if CV_NEON64
    asm(
            "sshll	%[res0].4s, %[reg].4h, #0\n\t"
            "sshll2 %[res1].4s, %[reg].8h, #0\n\t"
            : [res0] "=w" (res.val[0]), [res1] "=w" (res.val[1])
            : [reg] "w" (reg)
            :
       );
#elif CV_NEON
    asm(
            "vmovl.s16 %q[res0], %e[reg]\n\t"
            "vmovl.s16 %q[res1], %f[reg]\n\t"
            : [res0] "=w" (res.val[0]), [res1] "=w" (res.val[1])
            : [reg] "w" (reg)
            :
       );
#else
    res.val[0] = vmovl_s16(vget_low_s16(reg));
    res.val[1] = vmovl_s16(vget_high_s16(reg));
#endif
    return res;
}


#endif //CV_NEON
#endif //_OPENCV_NEON_FUNCTIONS_HPP_
//SRP0L 2013
