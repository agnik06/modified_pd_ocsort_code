#ifndef UNALIGNED_HPP
#define UNALIGNED_HPP

#include <stdint.h>

namespace cv {

#ifdef __GNUC__
typedef int16_t     unaligned_s16 __attribute__((aligned(1)));
typedef int32_t     unaligned_s32 __attribute__((aligned(1)));
typedef int64_t     unaligned_s64 __attribute__((aligned(1)));
typedef uint16_t    unaligned_u16 __attribute__((aligned(1)));
typedef uint32_t    unaligned_u32 __attribute__((aligned(1)));
typedef uint64_t    unaligned_u64 __attribute__((aligned(1)));
typedef float       unaligned_f32 __attribute__((aligned(1)));
typedef double      unaligned_f64 __attribute__((aligned(1)));
#else
template <class T>
struct unaligned
{
    unaligned() {}
    unaligned(const T &t)
    {
        *this = *reinterpret_cast<const unaligned *>(&t);
    }
    operator T() const
    {
        T t;
        *reinterpret_cast<unaligned *>(&t) = *this;
        return t;
    }
    uint8_t data[sizeof(T)];
};
typedef unaligned<int16_t> unaligned_s16;
typedef unaligned<int32_t> unaligned_s32;
typedef unaligned<int64_t> unaligned_s64;
typedef unaligned<uint16_t> unaligned_u16;
typedef unaligned<uint32_t> unaligned_u32;
typedef unaligned<uint64_t> unaligned_u64;
typedef unaligned<float> unaligned_f32;
typedef unaligned<double> unaligned_f64;
#endif

} // cv::

#endif // UNALIGNED_HPP
