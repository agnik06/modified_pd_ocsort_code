#ifndef __OPENCV_CORE_HMAT_HPP__
#define __OPENCV_CORE_HMAT_HPP__

#ifndef __cplusplus
#  error hmat.hpp header must be compiled as C++
#endif

#include "opencv2/core/mat.hpp"

#define CV_HMAT_MAX_DIM 2
#define CV_HMAT_TAILBYTE(size) ( (size + (4 * ALLOC_ROUNDUP-1)) & ~(4 * ALLOC_ROUNDUP-1) )
#define CV_PADW(alignment, cols, elemsize) ( (((alignment) - (((cols) * (elemsize)) % (alignment))) % (alignment)) )

namespace cv
{

CV_EXPORTS void* ionAlloc(size_t size);
CV_EXPORTS void ionFree(void* ptr);
CV_EXPORTS int ionGetfd(void* ptr);

////////////////////////////////// HMat //////////////////////////////////////////////

class CV_EXPORTS HMat
{
public:

    //! get ION Allocator according to architecture(Snapdragon or Exynos)
    static MatAllocator * getSpecificAllocator();

    //! get allocator (and init it if necessary)
    static MatAllocator * getHAllocator();

    //! return ION fd from virtual address
    int getfd();

    //! return Default Alignment of HMat
    static int getDefaultAlignment();

    //! default constructor
    HMat(MatAllocator* allocator = getSpecificAllocator());
    //! equivalent to Mat(rows, cols, type)
    HMat(int rows, int cols, int type, int alignment = getDefaultAlignment(), MatAllocator* allocator = getSpecificAllocator());
    //! equivalent to Mat(size, type)
    HMat(Size size, int type, int alignment = getDefaultAlignment(), MatAllocator* allocator = getSpecificAllocator());
    //! constructor that sets each matrix element to specified value
    HMat(int rows, int cols, int type, const Scalar& s, int alignment = getDefaultAlignment(), MatAllocator* allocator = getSpecificAllocator());
    //! equivalent to Mat(size, type, s)
    HMat(Size size, int type, const Scalar& s, int alignment = getDefaultAlignment(), MatAllocator* allocator = getSpecificAllocator());
    //! n-dim array constructor
    HMat(int ndims, const int* sizes, int type, int alignment = getDefaultAlignment(), MatAllocator* allocator = getSpecificAllocator());
    //! n-dim array constructor that sets each matrix element to specified value
    HMat(int ndims, const int* sizes, int type, const Scalar& s, int alignment = getDefaultAlignment(), MatAllocator* allocator = getSpecificAllocator());
    //! copy/conversion contructor. If m is of different type, it's going to be DEEP COPIED.
    HMat(const Mat& m, int alignment = getDefaultAlignment());
    //! copy constructor
    HMat(const HMat& m);
    //! constructs a matrix on top of user-allocated data. step is in bytes(!!!), regardless of the type
    HMat(int rows, int cols, int type, void* data, int alignment);
    //! equivalent to Mat(size, type, data, step)
    HMat(Size size, int type, void* data, int alignment);
    //! equivalent to Mat(ndims, sizes, type, data, steps)
    HMat(int ndims, const int* sizes, int type, void* data, int alignment);
    //! selects a submatrix
    HMat(const HMat& m, const Range& rowRange, const Range& colRange=Range::all());
    //! selects a submatrix
    HMat(const HMat& m, const Rect& roi);
    //! selects a submatrix, n-dim version
    HMat(const HMat& m, const Range* ranges);

    //! equivalent to Mat vector construct template from STL vector.
    //template<typename _Tp> explicit HMat(const std::vector<_Tp>& vec, bool copyData=false);

    /** @overload
    */
    //template<typename _Tp, int n> explicit HMat(const Vec<_Tp, n>& vec, bool copyData=true);

    /** @overload
    */
    //template<typename _Tp, int m, int n> explicit HMat(const Matx<_Tp, m, n>& mtx, bool copyData=true);

    /** @overload
    */
    //template<typename _Tp> explicit HMat(const Point_<_Tp>& pt, bool copyData=true);

    /** @overload
    */
    //template<typename _Tp> explicit HMat(const Point3_<_Tp>& pt, bool copyData=true);

    //! destructor - calls release()
    ~HMat();

    //! equivalent to Mat& operator = (const Mat& m)
    HMat& operator = (const HMat& m);

    //! equivalent to Mat& operator = (const MatExpr& expr)
    //HMat& operator = (const MatExpr& expr); //FIXME

    //! equivalent to Mat row(int y) const;
    HMat row(int y) const;

    //! equivalent to Mat col(int x) const;
    HMat col(int x) const;

    //! equivalent to Mat rowRange(int startrow, int endrow) const;
    HMat rowRange(int startrow, int endrow) const;

    //! equivalent to Mat rowRange(const Range& r) const;
    HMat rowRange(const Range& r) const;

    //! equivalent to Mat colRange(int startcol, int endcol) const;
    HMat colRange(int startcol, int endcol) const;

    //! equivalent to Mat colRange(const Range& r) const;
    HMat colRange(const Range& r) const;

    //! equivalent to Mat clone() const;
    HMat clone() const;

    //! equivalent to void copyTo( OutputArray m ) const;
    void copyTo( OutputArray m, int alignment = -1 ) const;

    //! equivalent to void convertTo( OutputArray m, int rtype, double alpha=1, double beta=0 ) const;
    //void convertTo( OutputArray m, int rtype, double alpha=1, double beta=0 ) const; //FIXME

    //! equivalent to void assignTo( Mat& m, int type=-1 ) const; //FIXME
    void assignTo( HMat& m, int type=-1 ) const;

    //! equivalent to Mat& operator = (const Scalar& s)
    HMat& operator = (const Scalar& s);

    //! equivalent to Mat& setTo(InputArray value, InputArray mask=noArray());
    HMat& setTo(InputArray value, InputArray mask=noArray()); //FIXME can it be supported or not?????
    //HMat& setTo(Scalar s);

    //! Matlab-stype matrix initialization //FIXME
    //static MatExpr zeros(int rows, int cols, int type);
    //static MatExpr zeros(Size size, int type);
    //static MatExpr zeros(int ndims, const int* sz, int type);
    //static MatExpr ones(int rows, int cols, int type);
    //static MatExpr ones(Size size, int type);
    //static MatExpr ones(int ndims, const int* sz, int type);
    //static MatExpr eye(int rows, int cols, int type);
    //static MatExpr eye(Size size, int type);

    //! overriden method, create
    void create(int rows, int cols, int type, int alignment = getDefaultAlignment());

    //! overriden method, create
    void create(Size size, int type, int alignment = getDefaultAlignment());

    //! overriden method, create
    void create(int ndims, const int* sizes, int type, int alignment = getDefaultAlignment());

    //! equivalent to void addref();
    void addref();

    //! overriden method, release HMat
    void release();

    //! overriden method, deallocates the matrix data
    void deallocate();

    //! equivalent to void copySize(const Mat& m);
    void copySize(const HMat& m);

    //! equivalent to Mat::reserve(size_t sz)
    //void reserve(size_t sz);

    //! equivalent to Mat::resize(size_t sz)
    //void resize(size_t sz);

    //! equivalent to Mat::resize(size_t sz, const Scalar& s)
    //void resize(size_t sz, const Scalar& s);

    //! upload src to HMat
    void upload(InputArray src, int alignment);

    //! download dst to HMat
    void download(OutputArray dst) const;

    //! returns allocated buffer size, which should be mapped to device
    size_t allocatedSize() const;

    //! flush caches on host side
    void flushCaches() const;

    //! invalidate caches on host side
    void invalidateCaches() const;

    //! flush and invalidate caches on host side
    void flushInvalidateCaches() const;

     //! getMat
    Mat getMat() const;

    //! equivalent to Mat operator()(rowRange, colRange)
    HMat operator()( Range rowRange, Range colRange ) const;

    //! equivalent to Mat operator()(roi)
    HMat operator()( const Rect& roi ) const;

    //! equivalent to Mat operator()(ranges)
    HMat operator()( const Range* ranges ) const;

    //! equivalent to Mat template<typename _Tp> operator std::vector<_Tp>() const
    //template<typename _Tp> operator std::vector<_Tp>() const;
    //! equivalent to Mat template<typename _Tp, int n> operator Vec<_Tp, n>() const
    //template<typename _Tp, int n> operator Vec<_Tp, n>() const;
    //! equivalent to template<typename _Tp, int m, int n> operator Matx<_Tp, m, n>() const
    //template<typename _Tp, int m, int n> operator Matx<_Tp, m, n>() const;

    //! equivalent to bool isContinuous() const;
    bool isContinuous() const;

    //! equivalent to bool isSubmatrix() const;
    bool isSubmatrix() const;

    //! equivalent to size_t elemSize() const;
    size_t elemSize() const;

    //! equivalent to size_t elemSize1() const;
    size_t elemSize1() const;

    //! equivalent to int type() const;
    int type() const;
    //! equivalent to int depth() const;
    int depth() const;

    //! equivalent to int channels() const;
    int channels() const;

    //! equivalent to size_t step1(int i=0) const;
    size_t step1(int i=0) const;

    //! equivalent to bool empty() const;
    bool empty() const;

    //! equivalent to size_t total() const;
    size_t total() const;

    uchar* ptr(int i0=0);
    /** @overload */
    const uchar* ptr(int i0=0) const;

    /** @overload */
    uchar* ptr(int i0, int i1);
    /** @overload */
    const uchar* ptr(int i0, int i1) const;

    /** @overload */
    uchar* ptr(int i0, int i1, int i2);
    /** @overload */
    const uchar* ptr(int i0, int i1, int i2) const;

    /** @overload */
    uchar* ptr(const int* idx);
    /** @overload */
    const uchar* ptr(const int* idx) const;
    /** @overload */
    template<int n> uchar* ptr(const Vec<int, n>& idx);
    /** @overload */
    template<int n> const uchar* ptr(const Vec<int, n>& idx) const;

    /** @overload */
    template<typename _Tp> _Tp* ptr(int i0=0);
    /** @overload */
    template<typename _Tp> const _Tp* ptr(int i0=0) const;
    /** @overload */
    template<typename _Tp> _Tp* ptr(int i0, int i1);
    /** @overload */
    template<typename _Tp> const _Tp* ptr(int i0, int i1) const;
    /** @overload */
    template<typename _Tp> _Tp* ptr(int i0, int i1, int i2);
    /** @overload */
    template<typename _Tp> const _Tp* ptr(int i0, int i1, int i2) const;
    /** @overload */
    template<typename _Tp> _Tp* ptr(const int* idx);
    /** @overload */
    template<typename _Tp> const _Tp* ptr(const int* idx) const;
    /** @overload */
    template<typename _Tp, int n> _Tp* ptr(const Vec<int, n>& idx);
    /** @overload */
    template<typename _Tp, int n> const _Tp* ptr(const Vec<int, n>& idx) const;


    template<typename _Tp> _Tp& at(int i0=0);
    /** @overload
      @param i0 Index along the dimension 0
      */
    template<typename _Tp> const _Tp& at(int i0=0) const;
    /** @overload
      @param i0 Index along the dimension 0
      @param i1 Index along the dimension 1
      */
    template<typename _Tp> _Tp& at(int i0, int i1);
    /** @overload
      @param i0 Index along the dimension 0
      @param i1 Index along the dimension 1
      */
    template<typename _Tp> const _Tp& at(int i0, int i1) const;

    /** @overload
      @param i0 Index along the dimension 0
      @param i1 Index along the dimension 1
      @param i2 Index along the dimension 2
      */
    template<typename _Tp> _Tp& at(int i0, int i1, int i2);
    /** @overload
      @param i0 Index along the dimension 0
      @param i1 Index along the dimension 1
      @param i2 Index along the dimension 2
      */
    template<typename _Tp> const _Tp& at(int i0, int i1, int i2) const;

    /** @overload
      @param idx Array of Mat::dims indices.
      */
    template<typename _Tp> _Tp& at(const int* idx);
    /** @overload
      @param idx Array of Mat::dims indices.
      */
    template<typename _Tp> const _Tp& at(const int* idx) const;

    /** @overload */
    template<typename _Tp, int n> _Tp& at(const Vec<int, n>& idx);
    /** @overload */
    template<typename _Tp, int n> const _Tp& at(const Vec<int, n>& idx) const;

    /** @overload
      special versions for 2D arrays (especially convenient for referencing image pixels)
      @param pt Element position specified as Point(j,i) .
      */
    template<typename _Tp> _Tp& at(Point pt);
    /** @overload
      special versions for 2D arrays (especially convenient for referencing image pixels)
      @param pt Element position specified as Point(j,i) .
      */
    template<typename _Tp> const _Tp& at(Point pt) const;

    enum { MAGIC_VAL  = 0x42FF0000, AUTO_STEP = 0, CONTINUOUS_FLAG = CV_MAT_CONT_FLAG, SUBMATRIX_FLAG = CV_SUBMAT_FLAG };
    enum { MAGIC_MASK = 0xFFFF0000, TYPE_MASK = 0x00000FFF, DEPTH_MASK = 7 };

    /*! includes several bit-fields:
      - the magic signature
      - continuity flag
      - depth
      - number of channels
      */
    int flags;
    //! the matrix dimensionality, >= 2
    int dims;
    //! the number of rows and columns or (-1, -1) when the matrix has more than 2 dimensions
    int rows, cols;
    //! the alignment of HMat
    int alignment;
    //! the tailbyte of HMat
    int tailbyte;
    //! pointer to the data
    uchar* data;

    //! helper fields used in locateROI and adjustROI
    const uchar* datastart;
    const uchar* dataend;
    const uchar* datalimit;

    //! custom allocator
    MatAllocator* allocator;

   //! interaction with UMat
    UMatData* u;

    MatSize size;
    MatStep step;
};

////////////////////////////////// HAllocator ////////////////////////////////////////

class CV_EXPORTS HAllocator : public MatAllocator
{
public:
    HAllocator();
    ~HAllocator();
    virtual UMatData* allocate(int dims, const int* sizes, int type,
                    void* data, size_t* step, int flags, UMatUsageFlags usageFlags) const = 0;
    virtual UMatData* allocate(int dims, const int* sizes, int type, int alignment,
                    int* tailbyte, void* data, size_t* step, int flags, UMatUsageFlags usageFlags) const = 0;
    virtual bool allocate(UMatData* data, int accessflags, UMatUsageFlags usageFlags ) const = 0;
    virtual void deallocate(UMatData* data) const = 0;
    virtual size_t allocatedSize(const HMat* mat) const = 0;
    //! Flushes caches on host side
    virtual void flushCaches(const HMat* mat) = 0;
    //! Invalidates caches on host side
    virtual void invalidateCaches(const HMat* mat) = 0;
    //! Flushes and invalidates caches on host side
    virtual void flushInvalidateCaches(const HMat* mat) = 0;

    virtual void unmap(UMatData* u) const = 0;

};


////////////////////////////////// HMatConstIterator /////////////////////////////////

class CV_EXPORTS HMatConstIterator
{
public:
    typedef uchar* value_type;
    typedef ptrdiff_t difference_type;
    typedef const uchar** pointer;
    typedef uchar* reference;

    //! default constructor
    HMatConstIterator();
    //! constructor that sets the iterator to the beginning of the hmatrix
    HMatConstIterator(const HMat* _hm);
    //! constructor that sets the iterator to the specified element of the hmatrix
    HMatConstIterator(const HMat* _hm, int _row, int _col=0);
    //! constructor that sets the iterator to the specified element of the hmatrix
    HMatConstIterator(const HMat* _hm, Point _pt);
    //! constructor that sets the iterator to the specified element of the hmatrix
    HMatConstIterator(const HMat* _hm, const int* _idx);
    //! copy constructor
    HMatConstIterator(const HMatConstIterator& it);

    //! copy operator
    HMatConstIterator& operator = (const HMatConstIterator& it);
    //! returns the current hmatrix element
    const uchar* operator *() const;
    //! returns the i-th hmatrix element, relative to the current
    const uchar* operator [](ptrdiff_t i) const;

    //! shifts the iterator forward by the specified number of elements
    HMatConstIterator& operator += (ptrdiff_t ofs);
    //! shifts the iterator backward by the specified number of elements
    HMatConstIterator& operator -= (ptrdiff_t ofs);
    //! decrements the iterator
    HMatConstIterator& operator --();
    //! decrements the iterator
    HMatConstIterator operator --(int);
    //! increments the iterator
    HMatConstIterator& operator ++();
    //! increments the iterator
    HMatConstIterator operator ++(int);
    //! returns the current iterator position
    Point pos() const;
    //! returns the current iterator position
    void pos(int* _idx) const;

    ptrdiff_t lpos() const;
    void seek(ptrdiff_t ofs, bool relative = false);
    void seek(const int* _idx, bool relative = false);

    const HMat* hm;
    size_t elemSize;
    const uchar* ptr;
    const uchar* sliceStart;
    const uchar* sliceEnd;
};

class CV_EXPORTS NAryHMatIterator
{
public:
    //! the default constructor
    NAryHMatIterator();
    //! the full constructor taking arbitrary number of n-dim matrices
    NAryHMatIterator(const HMat** arrays, uchar** ptrs, int narrays=-1);
    //! the full constructor taking arbitrary number of n-dim matrices
    NAryHMatIterator(const HMat** arrays, HMat* planes, int narrays=-1);
    //! the separate iterator initialization method
    void init(const HMat** arrays, HMat* planes, uchar** ptrs, int narrays=-1);

    //! proceeds to the next plane of every iterated hmatrix
    NAryHMatIterator& operator ++();
    //! proceeds to the next plane of every iterated hmatrix (postfix increment operator)
    NAryHMatIterator operator ++(int);

    //! the iterated arrays
    const HMat** arrays;
    //! the current planes
    HMat* planes;
    //! data pointers
    uchar** ptrs;
    //! the number of arrays
    int narrays;
    //! the number of hyper-planes that the iterator steps through
    size_t nplanes;
    //! the size of each segment (in elements)
    size_t size;
protected:
    int iterdepth;
    size_t idx;
};


} //cv

#include "opencv2/core/hmat.inl.hpp"


#endif
