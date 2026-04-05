#ifndef VEC_EXPR_HPP
#define VEC_EXPR_HPP

#include "vec_traits.hpp"

namespace cv {
namespace ex {

template <typename T>
using int_t = typename cv::V_TypeTraits<T>::type;

template <typename T>
using uint_t = typename cv::V_TypeTraits<T>::type;

template <class T, class E>
struct VecExpr
{
    typedef T value_type;
    typedef E type;
    typedef typename V_RegTrait128<T>::reg vector_type;

    inline E &This() { return static_cast<E&>(*this); }
    inline const E &This() const { return static_cast<const E&>(*this); }


    inline operator vector_type() const
    {
        return This().template Evaluate<vector_type>();
    }

    inline operator T() const
    {
        return This().template Evaluate<T>();
    }
};

#define APPLY_BINOP(op) (((V)L) op ((V)R))

#define VEC_BINOP_EXPR(NAME, op)\
template <class T, class LHS, class RHS>\
struct Vec##NAME##Expr : VecExpr<T, Vec##NAME##Expr<T, LHS, RHS> >\
{\
    typedef Vec##NAME##Expr type;\
    Vec##NAME##Expr(const LHS &l, const RHS &r) : L(l), R(r) {}\
    \
    LHS L;\
    RHS R;\
    \
    template <class V>\
    inline V Evaluate() const\
    {\
        return APPLY_BINOP(op);\
    }\
};\
template <class T, class LHS, class RHS>\
inline Vec##NAME##Expr<T, LHS, RHS> operator op(const VecExpr<T, LHS> &l, const VecExpr<T, RHS> &r)\
{\
    return { l.This(), r.This() };\
}

VEC_BINOP_EXPR(Add, +)
VEC_BINOP_EXPR(Sub, -)
VEC_BINOP_EXPR(Mul, *)
VEC_BINOP_EXPR(Div, /)
VEC_BINOP_EXPR(Shl, <<)
VEC_BINOP_EXPR(Shr, >>)
VEC_BINOP_EXPR(BitAnd, &)
VEC_BINOP_EXPR(BitOr, |)
VEC_BINOP_EXPR(BitXor, ^)

template <class T, bool isVector = VecTraits<T>::is_vector>
struct VecCmpProxy
{
    typedef T type;
};

template <class T>
struct VecCmpProxy<T, false>
{
    VecCmpProxy(const T &v) : value(v) {}
    T value;

    typedef VecCmpProxy<T> type;


    inline operator T() const { return value; }

    inline unsigned operator<(const VecCmpProxy<T> &rhs) const
    {
        return value < rhs.value ? (unsigned)-1 : 0;
    }
    inline unsigned operator>(const VecCmpProxy<T> &rhs) const
    {
        return value > rhs.value ? (unsigned)-1 : 0;
    }
    inline unsigned operator<=(const VecCmpProxy<T> &rhs) const
    {
        return value <= rhs.value ? (unsigned)-1 : 0;
    }
    inline unsigned operator>=(const VecCmpProxy<T> &rhs) const
    {
        return value >= rhs.value ? (unsigned)-1 : 0;
    }
    inline unsigned operator==(const VecCmpProxy<T> &rhs) const
    {
        return value == rhs.value ? (unsigned)-1 : 0;
    }
    inline unsigned operator!=(const VecCmpProxy<T> &rhs) const
    {
        return value != rhs.value ? (unsigned)-1 : 0;
    }
};

template <class T>
typename VecCmpProxy<T>::type vec_cmp(const T &x) { return x; }

#undef APPLY_BINOP
#define APPLY_BINOP(op) (typename VecCmpProxy<V>::type(L)) op (typename VecCmpProxy<V>::type(R))
VEC_BINOP_EXPR(LT, <)
VEC_BINOP_EXPR(GT, >)
VEC_BINOP_EXPR(LE, <=)
VEC_BINOP_EXPR(GE, >=)
VEC_BINOP_EXPR(EQ, ==)
VEC_BINOP_EXPR(NE, !=)

#undef APPLY_BINOP
#undef VEC_BINOP_EXPR

#define APPLY_UNOP(op) (op ((V)A))

#define VEC_UNOP_EXPR(NAME, op)\
template <class T, class Arg>\
struct Vec##NAME##Expr : VecExpr<T, Vec##NAME##Expr<T, Arg> >\
{\
    typedef Vec##NAME##Expr type;\
    Vec##NAME##Expr(const Arg &a) : A(a) {}\
    \
    Arg A;\
    \
    template <class V>\
    inline V Evaluate() const\
    {\
        return APPLY_UNOP(op);\
    }\
};\
template <class T, class Arg>\
inline Vec##NAME##Expr<T, Arg> operator op(const VecExpr<T, Arg> &arg)\
{\
    return { arg.This() };\
}

template <class T, class Arg>
inline const VecExpr<T, Arg> &operator +(const VecExpr<T, Arg> &arg)
{
    return arg;
}


VEC_UNOP_EXPR(Minus, -)
VEC_UNOP_EXPR(Compl, ~)
VEC_UNOP_EXPR(Not, !)

#undef APPLY_UNOP
#undef VEC_UNOP_EXPR

template <class T, class U = T>
using if_vector_t = typename std::enable_if<VecTraits<T>::is_vector, U>::type;

template <class T, class U>
using if_same_t = typename std::enable_if<std::is_same<T, U>::value, U>::type;

template <class T, class U = T>
using if_not_vector_t = typename std::enable_if<!VecTraits<T>::is_vector, U>::type;

template <class T>
struct SetallExpr : VecExpr<T, SetallExpr<T> >
{
    SetallExpr(const T &value) : v(value) {}

    T v;

    template <class U>
    inline if_same_t<T, U> Evaluate() const { return v; }

    template <class U>
    if_vector_t<U> Evaluate() const;
};

#define SETALL_IMPL(T, suffix)\
template <>\
template <>\
inline V_RegTrait128<T>::reg SetallExpr<T>::Evaluate<V_RegTrait128<T>::reg>() const { return v_setall_##suffix(v); }

SETALL_IMPL(uint8_t, u8)
SETALL_IMPL(uint16_t, u16)
SETALL_IMPL(uint32_t, u32)
SETALL_IMPL(uint64_t, u64)
SETALL_IMPL(int8_t, s8)
SETALL_IMPL(int16_t, s16)
SETALL_IMPL(int32_t, s32)
SETALL_IMPL(int64_t, s64)
SETALL_IMPL(float, f32)
#if CV_SIMD128_64F
SETALL_IMPL(double, f64)
#endif


template <class T>
struct LoadExpr : VecExpr<T, LoadExpr<T> >
{
    LoadExpr(const T *p) : ptr(p) {}

    const T *ptr;

    template <class U>
    inline if_same_t<T, U> Evaluate() const { return *ptr; }

    template <class U>
    inline if_vector_t<U> Evaluate() const
    {
        return v_load(ptr);
    }
};

template <class T>
inline LoadExpr<T> load(const T *ptr)
{
    return { ptr };
}

template <class T>
inline void store(T *ptr, const typename V_RegTrait128<T>::reg &value)
{
    v_store(ptr, value);
}

template <class T>
inline void store(T *ptr, const T &value)
{
    *ptr = value;
}

template <class T>
inline SetallExpr<T> setall(const T &value)
{
    return { value };
}

template <class DstScalar>
struct ReinterpretImpl
{
    typedef typename V_RegTrait128<DstScalar>::reg vector_type;

    template <class SrcVector>
    struct Inner
    {
        static vector_type eval(SrcVector src)
        {
            union {
                vector_type out;
                SrcVector in;
            } u;
            u.in = src;
            return u.out;
        }
    };
};

template <class T, class V>
inline if_vector_t<V, typename V_RegTrait128<T>::reg> reinterpret_as(V vec)
{
    return ReinterpretImpl<T>::template Inner<V>::eval(vec);
}

template <class T, class V>
inline if_not_vector_t<V, T> reinterpret_as(V scalar)
{
    union {
        T out;
        V in;
    } u;
    u.in = scalar;
    return u.out;
}


#define VEC_REINTERPRET_IMPL(dst_type, suffix)\
template <> template <class V>\
struct ReinterpretImpl<dst_type>::Inner\
{\
    static vector_type eval(V v) { return v_reinterpret_as_##suffix(v); }\
}\

VEC_REINTERPRET_IMPL(int8_t,    s8);
VEC_REINTERPRET_IMPL(int16_t,   s16);
VEC_REINTERPRET_IMPL(int32_t,   s32);
VEC_REINTERPRET_IMPL(int64_t,   s64);
VEC_REINTERPRET_IMPL(uint8_t,   u8);
VEC_REINTERPRET_IMPL(uint16_t,  u16);
VEC_REINTERPRET_IMPL(uint32_t,  u32);
VEC_REINTERPRET_IMPL(uint64_t,  u64);
VEC_REINTERPRET_IMPL(float,     f32);
#if CV_SIMD128_64F
VEC_REINTERPRET_IMPL(double,    f64);
#endif

template <class T, class Arg>
struct ReinterpretExpr : VecExpr<T, Arg>
{
    ReinterpretExpr(const Arg &a) : arg(a) {}
    Arg arg;

    template <class U>
    inline if_same_t<U, T> Evaluate() const {
        union {
            U out;
            typename Arg::value_type in;
        } u;
        u.in = (typename Arg::value_type)arg;
        return u.out;
    }

    template <class U>
    inline typename std::enable_if<VecTraits<U>::is_vector && std::is_same<typename VecTraits<U>::scalar, T>::value, U>::type
    Evaluate() const
    {
        return reinterpret_as<T>((typename Arg::vector_type)arg.Evaluate());
    }
};

template <class T, class U, class Arg>
inline ReinterpretExpr<T, Arg> reinterpret_as(const VecExpr<U, Arg> &arg)
{
    return { arg.This() };
}

#define VEC_OR_SCALAR_REDUCE_OP(op)\
template <class T>\
inline if_not_vector_t<T> reduce_##op(const T &v)\
{\
    return v;\
}\
template <class T>\
inline if_vector_t<T, typename VecTraits<T>::scalar> reduce_##op(const T &v)\
{\
    typename VecTraits<T>::scalar s = v_reduce_##op(v);\
    return s;\
}

VEC_OR_SCALAR_REDUCE_OP(sum)
VEC_OR_SCALAR_REDUCE_OP(max)
VEC_OR_SCALAR_REDUCE_OP(min)

#undef VEC_OR_SCALAR_REDUCE_OP

#define VEC_DEFINE_FUNC_EXPR_1(NAME, SCALAR_EVAL, VECTOR_EVAL)\
    template <class T, class Arg1>\
    struct Vec##NAME##1Expr : VecExpr<T, Vec##NAME##1Expr<T, Arg1> >\
    {\
        Vec##NAME##1Expr(const Arg1 &a1) : arg1(a1) {}\
        Arg1 arg1;\
        template <class U>\
        inline if_same_t<U, T> Evaluate() const { return SCALAR_EVAL((typename Arg1::value_type)arg1); }\
        template <class U>\
        inline if_vector_t<U> Evaluate() const\
        {\
            return VECTOR_EVAL((typename Arg1::vector_type)arg1);\
        }\
    };

#define VEC_DEFINE_FUNC_1(NAME, SCALAR_EVAL, VECTOR_EVAL)\
    VEC_DEFINE_FUNC_EXPR_1(NAME, SCALAR_EVAL, VECTOR_EVAL)\
    template <class T, class Arg1>\
    inline Vec##NAME##1Expr<T, Arg1> NAME(const VecExpr<T, Arg1> &arg1)\
    { return { arg1.This() }; }

VEC_DEFINE_FUNC_1(sqrt, std::sqrt, v_sqrt)
VEC_DEFINE_FUNC_1(invsqrt, 1/std::sqrt, cv::v_invsqrt)
VEC_DEFINE_FUNC_1(rcp, 1/, cv::v_rcp)

VEC_DEFINE_FUNC_EXPR_1(abs, std::abs, v_abs)
VEC_DEFINE_FUNC_EXPR_1(floor, floor, v_floor)
VEC_DEFINE_FUNC_EXPR_1(ceil, ceil, v_ceil)

template <class T, class Arg1>
inline Vecabs1Expr<typename std::make_unsigned<typename std::enable_if<std::is_integral<T>::value, T>::type>::type, Arg1>
abs(const VecExpr<T, Arg1> &arg1)
{ return { arg1.This() }; }

template <class T, class Arg1>
inline typename std::enable_if<std::is_floating_point<T>::value, Vecabs1Expr<T, Arg1>>::type
abs(const VecExpr<T, Arg1> &arg1)
{ return { arg1.This() }; }

template <class Arg1>
inline Vecfloor1Expr<int, Arg1> floor(const VecExpr<float, Arg1> &arg1)
{
    return { arg1.This() };
}

template <class Arg1>
inline Vecceil1Expr<int, Arg1> ceil(const VecExpr<float, Arg1> &arg1)
{
    return { arg1.This() };
}

using std::abs;
using std::floor;
using std::ceil;
using std::sqrt;

inline float rcp(float x) { return 1.0f/x; }
inline double rcp(double x) { return 1.0/x; }

template <class T>
inline if_vector_t<T> rcp(const T &x) { return v_rcp(x); }

template <class T>
inline decltype(std::sqrt(T())) invsqrt(const T &x) { return 1/std::sqrt(x); }

template <class T>
inline if_vector_t<T> abs(const T &x) { return v_abs(x); }
template <class T>
inline if_vector_t<T> floor(const T &x) { return v_floor(x); }
template <class T>
inline if_vector_t<T> ceil(const T &x) { return v_ceil(x); }
template <class T>
inline if_vector_t<T> sqrt(const T &x) { return v_sqrt(x); }
template <class T>
inline if_vector_t<T> invsqrt(const T &x) { return v_invsqrt(x); }

template <class T>
inline if_vector_t<T> cond_mask(T cond, T val)
{
    return cond & val;
}

// ternary operator
template <class T>
inline if_vector_t<T> sel(T cond, T _if, T _else)
{
    return (cond & _if) | (~cond & _else);
}

template <class T>
inline if_not_vector_t<T> cond_mask(bool cond, T val)
{
    return cond ? val : 0;
}

template <class T>
inline if_not_vector_t<T> sel(bool cond, T _if, T _else)
{
    return cond ? _if : _else;
}

template <class T>
inline if_vector_t<T, bool> check_any(T vec)
{
    return v_check_any(vec);
}

template <class T>
inline if_not_vector_t<T, bool> check_any(T scalar)
{
    return !!scalar;
}

template <class T>
inline if_vector_t<T, bool> check_all(T vec)
{
    return v_check_all(vec);
}

template <class T>
inline if_not_vector_t<T, bool> check_all(T scalar)
{
    return !!scalar;
}

template <class T>
inline void load_deinterleave(const T *ptr, T &x, T &y)
{
    x = ptr[0];
    y = ptr[1];
}

template <class T>
inline void load_deinterleave(const T *ptr, T &x, T &y, T &z)
{
    x = ptr[0];
    y = ptr[1];
    z = ptr[2];
}

template <class T>
inline void load_deinterleave(const T *ptr, T &x, T &y, T &z, T &w)
{
    x = ptr[0];
    y = ptr[1];
    z = ptr[2];
    w = ptr[3];
}

template <class T>
inline void load_deinterleave(const T *ptr,
                       typename V_RegTrait128<T>::reg &x,
                       typename V_RegTrait128<T>::reg &y)
{
    v_load_deinterleave(ptr, x, y);
}

template <class T>
inline void load_deinterleave(const T *ptr,
                       typename V_RegTrait128<T>::reg &x,
                       typename V_RegTrait128<T>::reg &y,
                       typename V_RegTrait128<T>::reg &z)
{
    v_load_deinterleave(ptr, x, y, z);
}

template <class T>
inline void load_deinterleave(const T *ptr,
                       typename V_RegTrait128<T>::reg &x,
                       typename V_RegTrait128<T>::reg &y,
                       typename V_RegTrait128<T>::reg &z,
                       typename V_RegTrait128<T>::reg &w)
{
    v_load_deinterleave(ptr, x, y, z, w);
}


template <class T>
inline void store_interleave(T *ptr, const T &x, const T &y)
{
    ptr[0] = x;
    ptr[1] = y;
}

template <class T>
inline void store_interleave(T *ptr, const T &x, const T &y, const T &z)
{
    ptr[0] = x;
    ptr[1] = y;
    ptr[2] = z;
}

template <class T>
inline void store_interleave(T *ptr, const T &x, const T &y, const T &z, const T &w)
{
    ptr[0] = x;
    ptr[1] = y;
    ptr[2] = z;
    ptr[3] = w;
}

template <class T>
inline void store_interleave(T *ptr,
                       const typename V_RegTrait128<T>::reg &x,
                       const typename V_RegTrait128<T>::reg &y)
{
    v_store_interleave(ptr, x, y);
}

template <class T>
inline void store_interleave(T *ptr,
                       const typename V_RegTrait128<T>::reg &x,
                       const typename V_RegTrait128<T>::reg &y,
                       const typename V_RegTrait128<T>::reg &z)
{
    v_store_interleave(ptr, x, y, z);
}

template <class T>
inline void store_interleave(T *ptr,
                       const typename V_RegTrait128<T>::reg &x,
                       const typename V_RegTrait128<T>::reg &y,
                       const typename V_RegTrait128<T>::reg &z,
                       const typename V_RegTrait128<T>::reg &w)
{
    v_store_interleave(ptr, x, y, z, w);
}

template <class T>
inline if_vector_t<T> muladd(const T &a, const T &b, const T &c)
{
    return v_muladd(a, b, c);
}

template <class T>
inline if_not_vector_t<T> muladd(const T &a, const T &b, const T &c)
{
    return a*b + c;
}

/* TODO: Implement HAL for lane extraction
template <int n, class T>
inline typename VecTraits<T>::scalar lane(const T &v)
{
    static_assert(n < VecTraits<T>::lanes, "Lane index out of range");
}

template <int n, class T>
inline if_not_vector_t<T> lane(const T &v)
{
    static_assert(n == 0, "Scalar value has only one lane");
    return v;
}
*/

} // ex

using ex::operator !;
using ex::operator ~;
using ex::operator +;
using ex::operator -;
using ex::operator *;
using ex::operator /;
using ex::operator <<;
using ex::operator >>;
using ex::operator <;
using ex::operator >;
using ex::operator <=;
using ex::operator >=;
using ex::operator ==;
using ex::operator !=;
using ex::operator &;
using ex::operator |;
using ex::operator ^;

} // cv

#endif // VEC_EXPR_HPP
