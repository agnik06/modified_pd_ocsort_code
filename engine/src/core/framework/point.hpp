/**
 * @file   point.hpp
 * @brief  Defines the Point(x,y)
 *
 * Defines the Point(x,y) for an image
 */

#ifndef __CORE_POINT__
#define __CORE_POINT__
namespace vizpet{
class Point {
public:
    int x;
    int y;

    Point() : x(0), y(0) {}

    Point(int _x, int _y) : x(_x), y(_y) {}
};
} // viz
#endif //__CORE_POINT__
