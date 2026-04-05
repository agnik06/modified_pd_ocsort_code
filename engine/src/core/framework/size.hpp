/**
 * @file   size.hpp
 * @brief  Defines the structure and properties of Size
 *
 * Size maintains the width and height of image
 */

#ifndef __CORE_SIZE__
#define __CORE_SIZE__

#include <string>
namespace vizpet{
class Size {
public:
    int width;
    int height;

    inline bool isEmpty() {
        return width < 1 || height < 1;
    }

    Size() : width(0), height(0) {}

    Size(int _width, int _height) : width(_width), height(_height) {}
};
} // viz
#endif //__CORE_SIZE__
