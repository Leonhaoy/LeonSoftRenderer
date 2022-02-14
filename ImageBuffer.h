#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string>
using namespace glm;
class ImageBuffer {
public:
    ImageBuffer(int w = 500, int h = 500) : width(w), height(h) { buffer.resize(w * h * 4); }
    ~ImageBuffer() {}
    void Resize(const int& w, const int& h) {
        width = w;
        height = h;
        buffer.resize(w * h * 4);
    }

    void SetColorOfPixel(const int& x, const int& y, vec4 color) {
        unsigned char* p = buffer.data();
        int pixel = (y * width + x) * 4;
        *(p + pixel) = color.r;
        *(p + pixel + 1) = color.g;
        *(p + pixel + 2) = color.b;
        *(p + pixel + 3) = color.a;
    }
    unsigned char* Data() { return buffer.data(); }
    void Clear() {
        for (auto it = buffer.begin(); it != buffer.end(); ++it) {
            *it = 0;
            *(++it) = 0;
            *(++it) = 0;
            *(++it) = 255;
        }
    }

private:
    int width;
    int height;
    std::vector<unsigned char> buffer;
};

#endif  // IMAGEBUFFER_H
