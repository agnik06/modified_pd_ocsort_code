/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   tensors.hpp
 * Author: Biplab Ch Das
 *
 * Created on 23 February, 2017, 11:21 AM
 */

#ifndef TENSORS_HPP
#define TENSORS_HPP
#include <cassert>
template <typename T>
class Tensor4D {
  T* data_;

 public:
  Tensor4D(T* data, int N, int C, int H, int W) {
    this->data_ = data;
    this->N = N;
    this->C = C;
    this->H = H;
    this->W = W;
  }
  T& operator()(int n, int c, int h, int w) {
    assert(n < N && n >= 0);
    assert(c < C && c >= 0);
    assert(h < H && h >= 0);
    assert(w < W && w >= 0);
    return data_[n * (C * H * W) + c * (H * W) + h * W + w];
  }
  T operator()(int n, int c, int h, int w) const {
    assert(n < N && n >= 0);
    assert(c < C && c >= 0);
    assert(h < H && h >= 0);
    assert(w < W && w >= 0);
    return data_[n * (C * H * W) + c * (H * W) + h * W + w];
  }
  int N, C, H, W;
};
template <typename T>
class Tensor3D {
  T* data_;
  bool is_owner_;
 public:
  Tensor3D(int C, int H, int W) {
    this->data_ = new T[C*H*W]();
    this->C = C;
    this->H = H;
    this->W = W;
    this->is_owner_ = true;
  }
  Tensor3D(T* data, int C, int H, int W) {
    this->data_ = data;
    this->C = C;
    this->H = H;
    this->W = W;
    this->is_owner_ = false;
  }
  T& operator()(int c, int h, int w) {
    assert(c < C && c >= 0);
    assert(h < H && h >= 0);
    assert(w < W && w >= 0);
    return data_[c * (H * W) + h * W + w];
  }
  T operator()(int c, int h, int w) const {
    assert(c < C && c >= 0);
    assert(h < H && h >= 0);
    assert(w < W && w >= 0);
    return data_[c * (H * W) + h * W + w];
  }
  T* data() { return this->data_; }
  ~Tensor3D() { if(is_owner_) { delete[] data_; } }

  int C, H, W;
};

template <typename T>
class Tensor2D {
  T* data_;
  bool is_owner_ = false;

 public:
  Tensor2D(T* data, int H, int W) {
    this->data_ = data;
    this->H = H;
    this->W = W;
  }
  Tensor2D(int H, int W) {
    this->data_ = new T[H * W]();
    this->H = H;
    this->W = W;
    this->is_owner_ = true;
  }
  T& operator()(int h, int w) {
    //assert(h < H && h >= 0);
    //assert(w < W && w >= 0);
    return data_[h * W + w];
  }
  T operator()(int c, int h, int w) const {
    //assert(h < H && h >= 0);
    //assert(w < W && w >= 0);
    return data_[h * W + w];
  }
  T *data() { return this->data_; }
  T *row(unsigned idx) const { return (this->data_ + idx * W); }
  ~Tensor2D() {
    if (is_owner_) {
      delete[] data_;
    }
  }
  int H, W;
};

#endif /* TENSORS_HPP */