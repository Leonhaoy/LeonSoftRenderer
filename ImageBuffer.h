#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string>
using namespace glm;
class ImageBuffer {
public:
    ImageBuffer(int w = 500, int h = 500) : width_(w), height_(h) { buffer_.resize(w * h * 4); }
    ~ImageBuffer() {}
    void Resize(const int& w, const int& h) {
        width_ = w;
        height_ = h;
        buffer_.resize(w * h * 4);
    }

    void SetColorOfPixel(const int& x, const int& y, vec4 color) {
        if (x > width_ || x < 0 || y > height_ || y < 0) {
            return;
        }
        unsigned char* p = buffer_.data();
        int pixel = (y * width_ + x) * 4;
        *(p + pixel) = color.r;
        *(p + pixel + 1) = color.g;
        *(p + pixel + 2) = color.b;
        *(p + pixel + 3) = color.a;
    }
    unsigned char* Data() { return buffer_.data(); }
    void Clear() {
        for (auto it = buffer_.begin(); it != buffer_.end(); ++it) {
            *it = 0;
            *(++it) = 0;
            *(++it) = 0;
            *(++it) = 255;
        }
    }

private:
    int width_;
    int height_;
    std::vector<unsigned char> buffer_;
};

#include <QObject>
#endif  // IMAGEBUFFER_H
