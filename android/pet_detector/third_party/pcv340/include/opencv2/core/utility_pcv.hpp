#ifndef OPENCV_CORE_UTILITY_PCV_H
#define OPENCV_CORE_UTILITY_PCV_H

//! @cond IGNORED

namespace cv
{

enum PARALLEL_FOR_TYPE
{
    PF_DYNAMIC = 0,
    PF_STATIC,
    PF_GUIDED,
    PF_BARRIER,

    PF_DEFAULT = PF_DYNAMIC
};

typedef struct PARALLEL_FOR_INFO
{
    PARALLEL_FOR_INFO( )
    {
        nthreads = 0;
        sched = PF_DEFAULT;
        chunk = 0;
    }

    PARALLEL_FOR_INFO(int _nthreads, int _sched = PF_DEFAULT, int _chunk = 0 )
    {
        nthreads = _nthreads;
        sched = _sched;
        chunk = _chunk;
    }

    int nthreads;
    int sched;
    int chunk;
} PF_INFO;

namespace somp
{

class CV_EXPORTS Parallel
{
public:
    Parallel();
    ~Parallel();
    void addLoopBody( const Range & range, const ParallelLoopBody & body,
        double nstripes = -1, int sched = PF_DEFAULT, int chunk = 0 );
    void addLoopBody( const Range & range, const ParallelLoopBody & body,
        double nstripes, const PF_INFO& pfi );
    void addSync();
    void run( int nthread = 0 );
    static int getThreadNum();
    static int getNumProcs();
private:
    class Impl;
    Impl *impl;
};

//! Parallel data processor.
CV_EXPORTS void _parallel_for_(const Range& range, const ParallelLoopBody& body,
    double nstripes=-1., const PF_INFO& pfi = PF_INFO() );
CV_EXPORTS void _parallel_for_(const Range& range, const ParallelLoopBody& body,
    double nstripes, int nth );
CV_EXPORTS void parallel_for_(const Range& range, const ParallelLoopBody& body,
    double nstripes=-1, int nth=0 );

}} // namespace cv::somp

//! @endcond

#endif // #ifndef OPENCV_CORE_UTILITY_PCV_H
