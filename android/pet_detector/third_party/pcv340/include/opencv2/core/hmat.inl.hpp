#ifndef __OPENCV_CORE_HMATRIX_OPERATIONS_HPP__
#define __OPENCV_CORE_HMATRIX_OPERATIONS_HPP__ 1

#ifndef __cplusplus
#  error hmat.inl.hpp header must be compiled as C++
#endif

#include "opencv2/core/hmat.hpp"

namespace cv {

//===================================================================================
// Heterogeneous Mat
//===================================================================================

inline
HMat::HMat(MatAllocator* _allocator)
    : flags(MAGIC_VAL), dims(0), rows(0), cols(0), alignment(0), tailbyte(0), data(0), datastart(0), dataend(0),
      datalimit(0), allocator(_allocator), u(0), size(&rows)
{}

inline
HMat::HMat(int _rows, int _cols, int _type, int _alignment, MatAllocator* _allocator)
    : flags(MAGIC_VAL), dims(0), rows(0), cols(0), alignment(0), tailbyte(0), data(0), datastart(0), dataend(0),
      datalimit(0), allocator(_allocator), u(0), size(&rows)
{
    create(_rows, _cols, _type, _alignment);
}

inline
HMat::HMat(int _rows, int _cols, int _type, const Scalar& _s, int _alignment, MatAllocator* _allocator)
    : flags(MAGIC_VAL), dims(0), rows(0), cols(0), alignment(0), tailbyte(0), data(0), datastart(0), dataend(0),
      datalimit(0), allocator(_allocator), u(0), size(&rows)
{
    create(_rows, _cols, _type, _alignment);
    *this = _s;
}

inline
HMat::HMat(Size _sz, int _type, int _alignment, MatAllocator* _allocator)
    : flags(MAGIC_VAL), dims(0), rows(0), cols(0), alignment(0), tailbyte(0), data(0), datastart(0), dataend(0),
      datalimit(0), allocator(_allocator), u(0), size(&rows)
{
    create( _sz.height, _sz.width, _type, _alignment);
}

inline
HMat::HMat(Size _sz, int _type, const Scalar& _s, int _alignment, MatAllocator* _allocator)
    : flags(MAGIC_VAL), dims(0), rows(0), cols(0), alignment(0), tailbyte(0), data(0), datastart(0), dataend(0),
      datalimit(0), allocator(_allocator), u(0), size(&rows)
{
    create(_sz.height, _sz.width, _type, _alignment);
    *this = _s;
}

inline
HMat::HMat(int _dims, const int* _sz, int _type, int _alignment, MatAllocator* _allocator)
    : flags(MAGIC_VAL), dims(0), rows(0), cols(0), alignment(0), tailbyte(0), data(0), datastart(0), dataend(0),
      datalimit(0), allocator(_allocator), u(0), size(&rows)
{
    create(_dims, _sz, _type, _alignment);
}

inline
HMat::HMat(int _dims, const int* _sz, int _type, const Scalar& _s, int _alignment, MatAllocator* _allocator)
    : flags(MAGIC_VAL), dims(0), rows(0), cols(0), alignment(0), tailbyte(0), data(0), datastart(0), dataend(0),
      datalimit(0), allocator(_allocator), u(0), size(&rows)
{
    create(_dims, _sz, _type, _alignment);
    *this = _s;
}

inline
HMat::HMat(const HMat& m)
    : flags(m.flags), dims(m.dims), rows(m.rows), cols(m.cols), alignment(m.alignment), tailbyte(m.tailbyte),
      data(m.data), datastart(m.datastart), dataend(m.dataend), datalimit(m.datalimit), allocator(m.allocator),
      u(m.u), size(&rows)
{
    if( u )
        CV_XADD(&u->refcount, 1);
    if( m.dims <= 2 )
    {
        step[0] = m.step[0]; step[1] = m.step[1];
    }
    else
    {
        dims = 0;
        copySize(m);
    }
}

inline
HMat::HMat(int _rows, int _cols, int _type, void* _data, int _alignment)
    : flags(MAGIC_VAL + (_type & TYPE_MASK)), dims(2), rows(_rows), cols(_cols), alignment(_alignment),
      tailbyte(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows)
{

    int fd = ionGetfd(_data);

    if(fd == -1)
    {
        Mat m = Mat(_rows, _cols, _type, _data);
        upload(m, _alignment);
        return;
    }
    else
    {
        this->datastart = this->data = (uchar*)_data;
        allocator = getSpecificAllocator();
        size_t esz = CV_ELEM_SIZE(_type), esz1 = CV_ELEM_SIZE1(_type);
        size_t padding = 0;
        if(_alignment > 1)
            padding = CV_PADW(_alignment, cols, CV_ELEM_SIZE(_type));

        size_t minstep = _cols * esz + padding;

        if ( minstep % esz1 != 0)
        {
            CV_Error(Error::BadStep, "Step must be a multiple of esz1");
        }
        this->flags |= HMat::CONTINUOUS_FLAG;

        step[0] = minstep;
        step[1] = esz;
        datalimit = datastart + minstep * rows;
        dataend = datalimit - padding;
    }
}

inline
HMat::HMat(Size _sz, int _type, void* _data, int _alignment)
    : flags(MAGIC_VAL + (_type & TYPE_MASK)), dims(2), rows(_sz.height), cols(_sz.width), alignment(_alignment),
      tailbyte(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows)
{
    int fd = ionGetfd(_data);

    if(fd == -1)
    {
        Mat m = Mat(_sz, _type, _data);
        upload(m, _alignment);
        return;
    }
    else
    {
        this->datastart = this->data = (uchar*)_data;
        allocator = getSpecificAllocator();
        size_t esz = CV_ELEM_SIZE(_type), esz1 = CV_ELEM_SIZE1(_type);
        size_t padding = 0;
        if(_alignment > 1)
            padding = CV_PADW(_alignment, cols, CV_ELEM_SIZE(_type));

        size_t minstep = _sz.width * esz + padding;

        if ( minstep % esz1 != 0)
        {
            CV_Error(Error::BadStep, "Step must be a multiple of esz1");
        }
        this->flags |= HMat::CONTINUOUS_FLAG;

        step[0] = minstep;
        step[1] = esz;
        datalimit = datastart + minstep * rows;
        dataend = datalimit - padding;
    }
}

inline
HMat::~HMat()
{
    release();
    if( step.p != step.buf )
    {
        fastFree(step.p);
        step.p = NULL;
    }
}

inline
HMat& HMat::operator = (const HMat& m)
{
    if( this != &m )
    {
        if( m.u )
            CV_XADD(&m.u->refcount, 1);
        release();
        flags = m.flags;
        if( dims <= 2 && m.dims <= 2 )
        {
            dims = m.dims;
            rows = m.rows;
            cols = m.cols;
            step[0] = m.step[0];
            step[1] = m.step[1];
        }
        else
            copySize(m);
        data = m.data;
        datastart = m.datastart;
        dataend = m.dataend;
        datalimit = m.datalimit;
        allocator = m.allocator;
        alignment = m.alignment;
        tailbyte = m.tailbyte;
        u = m.u;
    }
    return *this;
}

inline
HMat HMat::row(int y) const
{
    return HMat(*this, Range(y, y + 1), Range::all());
}

inline
HMat HMat::col(int x) const
{
    return HMat(*this, Range::all(), Range(x, x + 1));
}

inline
HMat HMat::rowRange(int startrow, int endrow) const
{
    return HMat(*this, Range(startrow, endrow), Range::all());
}

inline
HMat HMat::rowRange(const Range& r) const
{
    return HMat(*this, r, Range::all());
}

inline
HMat HMat::colRange(int startcol, int endcol) const
{
    return HMat(*this, Range::all(), Range(startcol, endcol));
}

inline
HMat HMat::colRange(const Range& r) const
{
    return HMat(*this, Range::all(), r);
}

inline
HMat HMat::clone() const
{
    HMat m;
    copyTo(m);
    return m;
}

inline
void HMat::assignTo( HMat& m, int _type ) const
{
    if( _type < 0 )
        m = *this;
    else
        CV_Error(Error::StsError, "HMat doesnt support convertTo yet.");
}

inline
void HMat::create(int _rows, int _cols, int _type, int _alignment)
{
    _type &= TYPE_MASK;
    int _padw = 0;
    if( _alignment > 1 )
        _padw = CV_PADW(_alignment, _cols, CV_ELEM_SIZE(_type));

    if( dims <= 2 && rows == _rows && cols == _cols && type() == _type && data )
    {
        if(step.p && (step.p[0] == (size_t)(_padw + _cols * _type)))
            return;
    }
    int sz[] = {_rows, _cols};
    create(2, sz, _type, _alignment);
}

inline
void HMat::create(Size _sz, int _type, int _alignment)
{
    create(_sz.height, _sz.width, _type, _alignment);
}

inline
void HMat::addref()
{
    if( u )
        CV_XADD(&u->refcount, 1);
}

inline void HMat::release()
{
    if( u && CV_XADD(&u->refcount, -1) == 1 )
        deallocate();
    u = NULL;
    datastart = dataend = datalimit = data = 0;
    for(int i = 0; i < dims; i++)
        size.p[i] = 0;
}

inline
HMat HMat::operator()( Range _rowRange, Range _colRange ) const
{
    return HMat(*this, _rowRange, _colRange);
}

inline
HMat HMat::operator()( const Rect& roi ) const
{
    return HMat(*this, roi);
}

inline
HMat HMat::operator()(const Range* ranges) const
{
    return HMat(*this, ranges);
}

inline
bool HMat::isContinuous() const
{
    return (flags & CONTINUOUS_FLAG) != 0;
}

inline
bool HMat::isSubmatrix() const
{
    return (flags & SUBMATRIX_FLAG) != 0;
}

inline
size_t HMat::elemSize() const
{
    return dims > 0 ? step.p[dims - 1] : 0;
}

inline
size_t HMat::elemSize1() const
{
    return CV_ELEM_SIZE1(flags);
}

inline
int HMat::type() const
{
    return CV_MAT_TYPE(flags);
}

inline
int HMat::depth() const
{
    return CV_MAT_DEPTH(flags);
}

inline
int HMat::channels() const
{
    return CV_MAT_CN(flags);
}

inline
size_t HMat::step1(int i) const
{
    return step.p[i] / elemSize1();
}

inline
bool HMat::empty() const
{
    return data == 0 || total() == 0;
}

inline
size_t HMat::total() const
{
    if( dims <= 2 )
        return (size_t)rows * cols;
    size_t p = 1;
    for( int i = 0; i < dims; i++ )
        p *= size[i];
    return p;
}

inline
uchar* HMat::ptr(int y)
{
    CV_DbgAssert( y == 0 || (data && dims >= 1 && (unsigned)y < (unsigned)size.p[0]) );
    return data + step.p[0] * y;
}

inline
const uchar* HMat::ptr(int y) const
{
    CV_DbgAssert( y == 0 || (data && dims >= 1 && (unsigned)y < (unsigned)size.p[0]) );
    return data + step.p[0] * y;
}

template<typename _Tp> inline
_Tp* HMat::ptr(int y)
{
    CV_DbgAssert( y == 0 || (data && dims >= 1 && (unsigned)y < (unsigned)size.p[0]) );
    return (_Tp*)(data + step.p[0] * y);
}

template<typename _Tp> inline
const _Tp* HMat::ptr(int y) const
{
    CV_DbgAssert( y == 0 || (data && dims >= 1 && data && (unsigned)y < (unsigned)size.p[0]) );
    return (const _Tp*)(data + step.p[0] * y);
}

inline
uchar* HMat::ptr(int i0, int i1)
{
    CV_DbgAssert( dims >= 2 && data &&
                  (unsigned)i0 < (unsigned)size.p[0] &&
                  (unsigned)i1 < (unsigned)size.p[1] );
    return data + i0 * step.p[0] + i1 * step.p[1];
}

inline
const uchar* HMat::ptr(int i0, int i1) const
{
    CV_DbgAssert( dims >= 2 && data &&
                 (unsigned)i0 < (unsigned)size.p[0] &&
                 (unsigned)i1 < (unsigned)size.p[1] );
    return data + i0 * step.p[0] + i1 * step.p[1];
}

template<typename _Tp> inline
_Tp* HMat::ptr(int i0, int i1)
{
    CV_DbgAssert( dims >= 2 && data &&
                  (unsigned)i0 < (unsigned)size.p[0] &&
                  (unsigned)i1 < (unsigned)size.p[1] );
    return (_Tp*)(data + i0 * step.p[0] + i1 * step.p[1]);
}

template<typename _Tp> inline
const _Tp* HMat::ptr(int i0, int i1) const
{
    CV_DbgAssert( dims >= 2 && data &&
                  (unsigned)i0 < (unsigned)size.p[0] &&
                  (unsigned)i1 < (unsigned)size.p[1] );
    return (const _Tp*)(data + i0 * step.p[0] + i1 * step.p[1]);
}

inline
uchar* HMat::ptr(int i0, int i1, int i2)
{
    CV_DbgAssert( dims >= 3 && data &&
                  (unsigned)i0 < (unsigned)size.p[0] &&
                  (unsigned)i1 < (unsigned)size.p[1] &&
                  (unsigned)i2 < (unsigned)size.p[2] );
    return data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2];
}

inline
const uchar* HMat::ptr(int i0, int i1, int i2) const
{
    CV_DbgAssert( dims >= 3 && data &&
                  (unsigned)i0 < (unsigned)size.p[0] &&
                  (unsigned)i1 < (unsigned)size.p[1] &&
                  (unsigned)i2 < (unsigned)size.p[2] );
    return data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2];
}

template<typename _Tp> inline
_Tp* HMat::ptr(int i0, int i1, int i2)
{
    CV_DbgAssert( dims >= 3 && data &&
                  (unsigned)i0 < (unsigned)size.p[0] &&
                  (unsigned)i1 < (unsigned)size.p[1] &&
                  (unsigned)i2 < (unsigned)size.p[2] );
    return (_Tp*)(data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2]);
}

template<typename _Tp> inline
const _Tp* HMat::ptr(int i0, int i1, int i2) const
{
    CV_DbgAssert( dims >= 3 && data &&
                  (unsigned)i0 < (unsigned)size.p[0] &&
                  (unsigned)i1 < (unsigned)size.p[1] &&
                  (unsigned)i2 < (unsigned)size.p[2] );
    return (const _Tp*)(data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2]);
}

inline
uchar* HMat::ptr(const int* idx)
{
    int i, d = dims;
    uchar* p = data;
    CV_DbgAssert( d >= 1 && p );
    for( i = 0; i < d; i++ )
    {
        CV_DbgAssert( (unsigned)idx[i] < (unsigned)size.p[i] );
        p += idx[i] * step.p[i];
    }
    return p;
}

inline
const uchar* HMat::ptr(const int* idx) const
{
    int i, d = dims;
    uchar* p = data;
    CV_DbgAssert( d >= 1 && p );
    for( i = 0; i < d; i++ )
    {
        CV_DbgAssert( (unsigned)idx[i] < (unsigned)size.p[i] );
        p += idx[i] * step.p[i];
    }
    return p;
}

template<typename _Tp> inline
_Tp& HMat::at(int i0, int i1)
{
    CV_DbgAssert( dims <= 2 && data && (unsigned)i0 < (unsigned)size.p[0] &&
        (unsigned)(i1 * DataType<_Tp>::channels) < (unsigned)(size.p[1] * channels()) &&
        CV_ELEM_SIZE1(DataType<_Tp>::depth) == elemSize1());
    return ((_Tp*)(data + step.p[0] * i0))[i1];
}

template<typename _Tp> inline
const _Tp& HMat::at(int i0, int i1) const
{
    CV_DbgAssert( dims <= 2 && data && (unsigned)i0 < (unsigned)size.p[0] &&
        (unsigned)(i1 * DataType<_Tp>::channels) < (unsigned)(size.p[1] * channels()) &&
        CV_ELEM_SIZE1(DataType<_Tp>::depth) == elemSize1());
    return ((const _Tp*)(data + step.p[0] * i0))[i1];
}

template<typename _Tp> inline
_Tp& HMat::at(Point pt)
{
    CV_DbgAssert( dims <= 2 && data && (unsigned)pt.y < (unsigned)size.p[0] &&
        (unsigned)(pt.x * DataType<_Tp>::channels) < (unsigned)(size.p[1] * channels()) &&
        CV_ELEM_SIZE1(DataType<_Tp>::depth) == elemSize1());
    return ((_Tp*)(data + step.p[0] * pt.y))[pt.x];
}

template<typename _Tp> inline
const _Tp& HMat::at(Point pt) const
{
    CV_DbgAssert( dims <= 2 && data && (unsigned)pt.y < (unsigned)size.p[0] &&
        (unsigned)(pt.x * DataType<_Tp>::channels) < (unsigned)(size.p[1] * channels()) &&
        CV_ELEM_SIZE1(DataType<_Tp>::depth) == elemSize1());
    return ((const _Tp*)(data + step.p[0] * pt.y))[pt.x];
}

template<typename _Tp> inline
_Tp& HMat::at(int i0)
{
    CV_DbgAssert( dims <= 2 && data &&
                 (unsigned)i0 < (unsigned)(size.p[0] * size.p[1]) &&
                 elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    if( isContinuous() || size.p[0] == 1 )
        return ((_Tp*)data)[i0];
    if( size.p[1] == 1 )
        return *(_Tp*)(data + step.p[0] * i0);
    int i = i0 / cols, j = i0 - i * cols;
    return ((_Tp*)(data + step.p[0] * i))[j];
}

template<typename _Tp> inline
const _Tp& HMat::at(int i0) const
{
    CV_DbgAssert( dims <= 2 && data &&
                 (unsigned)i0 < (unsigned)(size.p[0] * size.p[1]) &&
                 elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    if( isContinuous() || size.p[0] == 1 )
        return ((const _Tp*)data)[i0];
    if( size.p[1] == 1 )
        return *(const _Tp*)(data + step.p[0] * i0);
    int i = i0 / cols, j = i0 - i * cols;
    return ((const _Tp*)(data + step.p[0] * i))[j];
}

template<typename _Tp> inline
_Tp& HMat::at(int i0, int i1, int i2)
{
    CV_DbgAssert( elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    return *(_Tp*)ptr(i0, i1, i2);
}

template<typename _Tp> inline
const _Tp& HMat::at(int i0, int i1, int i2) const
{
    CV_DbgAssert( elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    return *(const _Tp*)ptr(i0, i1, i2);
}

template<typename _Tp> inline
_Tp& HMat::at(const int* idx)
{
    CV_DbgAssert( elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    return *(_Tp*)ptr(idx);
}

template<typename _Tp> inline
const _Tp& HMat::at(const int* idx) const
{
    CV_DbgAssert( elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    return *(const _Tp*)ptr(idx);
}

template<typename _Tp, int n> inline
_Tp& HMat::at(const Vec<int, n>& idx)
{
    CV_DbgAssert( elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    return *(_Tp*)ptr(idx.val);
}

template<typename _Tp, int n> inline
const _Tp& HMat::at(const Vec<int, n>& idx) const
{
    CV_DbgAssert( elemSize() == CV_ELEM_SIZE(DataType<_Tp>::type) );
    return *(const _Tp*)ptr(idx.val);
}
/*
inline
HMat& HMat::operator = (const MatExpr& e)
{
    e.op->assign(e, *this);
    return *this;
}

template<typename _Tp> inline
HMat::operator std::vector<_Tp>() const
{
    std::vector<_Tp> v;
    copyTo(v);
    return v;
}

template<typename _Tp, int n> inline
HMat::operator Vec<_Tp, n>() const
{
    CV_Assert( data && dims <= 2 && (rows == 1 || cols == 1) &&
               rows + cols - 1 == n && channels() == 1 );

    if( isContinuous() && type() == DataType<_Tp>::type )
        return Vec<_Tp, n>((_Tp*)data);
    Vec<_Tp, n> v;
    HMat tmp(rows, cols, DataType<_Tp>::type, v.val);
    convertTo(tmp, tmp.type());
    return v;
}

template<typename _Tp, int m, int n> inline
HMat::operator Matx<_Tp, m, n>() const
{
    CV_Assert( data && dims <= 2 && rows == m && cols == n && channels() == 1 );

    if( isContinuous() && type() == DataType<_Tp>::type )
        return Matx<_Tp, m, n>((_Tp*)data);
    Matx<_Tp, m, n> mtx;
    HMat tmp(rows, cols, DataType<_Tp>::type, mtx.val);
    convertTo(tmp, tmp.type());
    return mtx;
}
*/
inline
size_t HMat::allocatedSize() const
{
    HAllocator * ha = dynamic_cast<HAllocator *>(allocator);
    return ha->allocatedSize(this);
}


//===================================================================================
// HMatConstIterator
//===================================================================================

inline
HMatConstIterator::HMatConstIterator()
    : hm(0), elemSize(0), ptr(0), sliceStart(0), sliceEnd(0)
{}

inline
HMatConstIterator::HMatConstIterator(const HMat* _hm)
    : hm(_hm), elemSize(_hm->elemSize()), ptr(0), sliceStart(0), sliceEnd(0)
{
    if( hm && hm->isContinuous() )
    {
        sliceStart = hm->ptr();
        sliceEnd = sliceStart + hm->total()*elemSize;
    }
    seek((const int*)0);
}

inline
HMatConstIterator::HMatConstIterator(const HMat* _hm, int _row, int _col)
    : hm(_hm), elemSize(_hm->elemSize()), ptr(0), sliceStart(0), sliceEnd(0)
{
    CV_Assert(hm && hm->dims <= 2);
    if( hm->isContinuous() )
    {
        sliceStart = hm->ptr();
        sliceEnd = sliceStart + hm->total()*elemSize;
    }
    int idx[] = {_row, _col};
    seek(idx);
}

inline
HMatConstIterator::HMatConstIterator(const HMat* _hm, Point _pt)
    : hm(_hm), elemSize(_hm->elemSize()), ptr(0), sliceStart(0), sliceEnd(0)
{
    CV_Assert(hm && hm->dims <= 2);
    if( hm->isContinuous() )
    {
        sliceStart = hm->ptr();
        sliceEnd = sliceStart + hm->total()*elemSize;
    }
    int idx[] = {_pt.y, _pt.x};
    seek(idx);
}

inline
HMatConstIterator::HMatConstIterator(const HMatConstIterator& it)
    : hm(it.hm), elemSize(it.elemSize), ptr(it.ptr), sliceStart(it.sliceStart), sliceEnd(it.sliceEnd)
{}

inline
HMatConstIterator& HMatConstIterator::operator = (const HMatConstIterator& it )
{
    hm = it.hm; elemSize = it.elemSize; ptr = it.ptr;
    sliceStart = it.sliceStart; sliceEnd = it.sliceEnd;
    return *this;
}

inline
const uchar* HMatConstIterator::operator *() const
{
    return ptr;
}

inline HMatConstIterator& HMatConstIterator::operator += (ptrdiff_t ofs)
{
    if( !hm || ofs == 0 )
        return *this;
    ptrdiff_t ofsb = ofs*elemSize;
    ptr += ofsb;
    if( ptr < sliceStart || sliceEnd <= ptr )
    {
        ptr -= ofsb;
        seek(ofs, true);
    }
    return *this;
}

inline
HMatConstIterator& HMatConstIterator::operator -= (ptrdiff_t ofs)
{
    return (*this += -ofs);
}

inline
HMatConstIterator& HMatConstIterator::operator --()
{
    if( hm && (ptr -= elemSize) < sliceStart )
    {
        ptr += elemSize;
        seek(-1, true);
    }
    return *this;
}

inline
HMatConstIterator HMatConstIterator::operator --(int)
{
    HMatConstIterator b = *this;
    *this += -1;
    return b;
}

inline
HMatConstIterator& HMatConstIterator::operator ++()
{
    if( hm && (ptr += elemSize) >= sliceEnd )
    {
        ptr -= elemSize;
        seek(1, true);
    }
    return *this;
}

inline HMatConstIterator HMatConstIterator::operator ++(int)
{
    HMatConstIterator b = *this;
    *this += 1;
    return b;
}

static inline
bool operator == (const HMatConstIterator& a, const HMatConstIterator& b)
{
    return a.hm == b.hm && a.ptr == b.ptr;
}

static inline
bool operator != (const HMatConstIterator& a, const HMatConstIterator& b)
{
    return !(a == b);
}

static inline
bool operator < (const HMatConstIterator& a, const HMatConstIterator& b)
{
    return a.ptr < b.ptr;
}

static inline
bool operator > (const HMatConstIterator& a, const HMatConstIterator& b)
{
    return a.ptr > b.ptr;
}

static inline
bool operator <= (const HMatConstIterator& a, const HMatConstIterator& b)
{
    return a.ptr <= b.ptr;
}

static inline
bool operator >= (const HMatConstIterator& a, const HMatConstIterator& b)
{
    return a.ptr >= b.ptr;
}

static inline
ptrdiff_t operator - (const HMatConstIterator& b, const HMatConstIterator& a)
{
    if( a.hm != b.hm )
        return ((size_t)(-1) >> 1);
    if( a.sliceEnd == b.sliceEnd )
        return (b.ptr - a.ptr)/b.elemSize;

    return b.lpos() - a.lpos();
}

static inline
HMatConstIterator operator + (const HMatConstIterator& a, ptrdiff_t ofs)
{
    HMatConstIterator b = a;
    return b += ofs;
}

static inline
HMatConstIterator operator + (ptrdiff_t ofs, const HMatConstIterator& a)
{
    HMatConstIterator b = a;
    return b += ofs;
}

static inline
HMatConstIterator operator - (const HMatConstIterator& a, ptrdiff_t ofs)
{
    HMatConstIterator b = a;
    return b += -ofs;
}


inline
const uchar* HMatConstIterator::operator [](ptrdiff_t i) const
{
    return *(*this + i);
}

} // cv namespace

#endif

