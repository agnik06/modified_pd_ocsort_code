// Copyright (c) 2017 Samsung R&D Poland
//
// Michal Zientkiewicz

#ifndef VEC_TRAITS_HPP
#define VEC_TRAITS_HPP

#include "intrin.hpp"

namespace cv {

template <class T>
struct SupportsSIMD128
{
#if !defined(CV_SIMD128) || !CV_SIMD128
    static const bool value = false;
#else
    template <class TT>
    static typename V_RegTrait128<TT>::reg _f(TT);

    template <class TT>
    static char _f(...);

    static const bool value = sizeof(_f<T>(T())) > 1;
#endif
    operator bool() const { return value; }
};

template <class T>
struct VecTraits;

template <unsigned width, unsigned vector_size = 0>
struct float_type {};

template <>
struct float_type<sizeof(float), 0>
{
    typedef float fp_type;
};

template <>
struct float_type<sizeof(float), 16>
{
    // on ndk r12b and API_LEVEL < 21, it doesn't have float_t type in math.h
    typedef V_RegTrait128<float>::reg fp_type;
};

template <>
struct float_type<sizeof(double), 0>
{
    typedef double fp_type;
};

#if CV_SIMD128_64F
template <>
struct float_type<sizeof(double), 16>
{
    typedef V_RegTrait128<double>::reg fp_type;
};
#endif

template <unsigned width, unsigned vector_size = 0>
struct int_type;

template <unsigned width, unsigned vector_size = 0>
struct uint_type;

template <> struct int_type<1> { typedef int8_t type; };\
template <> struct uint_type<1> { typedef uint8_t type; };
template <> struct int_type<2> { typedef int16_t type; };\
template <> struct uint_type<2> { typedef uint16_t type; };
template <> struct int_type<4> { typedef int32_t type; };\
template <> struct uint_type<4> { typedef uint32_t type; };
template <> struct int_type<8> { typedef int64_t type; };\
template <> struct uint_type<8> { typedef uint64_t type; };

template <unsigned width>
struct int_type<width, 16>
{
    typedef typename V_RegTrait128<typename int_type<width>::type>::reg type;
};
template <unsigned width>
struct uint_type<width, 16>
{
    typedef typename V_RegTrait128<typename uint_type<width>::type>::reg type;
};

#if __cplusplus >= 201103L
template <unsigned n, unsigned vector_size = 0>
using int_t = typename int_type<n, vector_size>::type;

template <unsigned n, unsigned vector_size = 0>
using uint_t = typename uint_type<n, vector_size>::type;

template <unsigned n, unsigned vector_size = 0>
using float_t = typename float_type<n, vector_size>::type;

template <class T>
using compatible_int_t = int_t<sizeof(typename VecTraits<T>::scalar), VecTraits<T>::is_vector ? sizeof(T) : 0u>;

template <class T>
using compatible_uint_t = uint_t<sizeof(typename VecTraits<T>::scalar), VecTraits<T>::is_vector ? sizeof(T) : 0u>;

template <class T>
using compatible_float_t = float_t<sizeof(typename VecTraits<T>::scalar), VecTraits<T>::is_vector ? sizeof(T) : 0u>;
#endif

template <class Scalar, int NumLanes>
struct VecTraitsBase
{
    static const bool is_vector = NumLanes > 1;
    static const int lanes = NumLanes;
    typedef Scalar scalar;
};

template <class T>
struct VecTraits : VecTraitsBase<T, 1>
{
};

#define DECLARE_VECTOR_128(type)\
template <>\
struct VecTraits<V_RegTrait128<type>::reg> : VecTraitsBase<type, V_SIMD128Traits<type>::nlanes> {}

DECLARE_VECTOR_128(schar);
DECLARE_VECTOR_128(uchar);
DECLARE_VECTOR_128(int16_t);
DECLARE_VECTOR_128(uint16_t);
DECLARE_VECTOR_128(int32_t);
DECLARE_VECTOR_128(uint32_t);
DECLARE_VECTOR_128(int64_t);
DECLARE_VECTOR_128(uint64_t);
DECLARE_VECTOR_128(float);
#if CV_SIMD128_64F
DECLARE_VECTOR_128(double);
#endif

} // cv

#endif // VEC_TRAITS_HPP
