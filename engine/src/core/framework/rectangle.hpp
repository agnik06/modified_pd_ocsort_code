/**
 * @file   rectangle.hpp
 * @brief  This file provides utilities for roi rectagle box
 *
 * This file provides utilities for roi rectagle box
 */

#ifndef __CORE_RECTANGLE__
#define __CORE_RECTANGLE__

#include "point.hpp"

namespace vizpet {
class Rectangle {
public:
    int bottom;
    int left;
    int right;
    int top;

    Rectangle(){}
    Rectangle(int _left, int _top, int _right, int _bottom){
        this->left = _left;
        this->top = _top;
        this->right = _right;
        this->bottom = _bottom;
    }

    int width(){
        return right-left+1;
    }
    int height(){
        return bottom-top+1;
    }
    int area(){
        if(isInvalid())
            return 0;
        return width() * height();
    }
    inline bool isInvalid(){
        return left < 0 || top < 0 || right < 1 || bottom < 1 || width() < 1 || height() < 1;
    }
    //Gets the center x
    int get_center_x() {
        return (this->right + this->left) / 2;
    }
    //Gets the center y coordinate
    int get_center_y() {
        return (this->bottom + this->top) / 2;
    }
    //Get the center point of rectangle
    Point get_center(){
        return vizpet::Point(get_center_x(), get_center_y());
    }
    //Checks if rectangle contains (x,y)
    bool contains(int x, int y) {
        return (x <= this->right && x >= this->left) &&
            (y <= this->bottom && y >= this->top);
    }
    //Checks if rectangle contains Point
    bool contains(Point point) {
        return contains(point.x, point.y);
    }
};
}
#endif