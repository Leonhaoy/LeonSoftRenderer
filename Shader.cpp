#include "Shader.h"
vec3 Shader::texture(vec2 uv, TGAImage& image) {
    int width = image.get_width();
    int height = image.get_height();
    float x = uv[0] * width;
    float y = (1 - uv[1]) * height;
    if ((x < 0) || (x > width - 1) || (y < 0) || (y > height - 1)) {
        return vec3(0.f);
    }
    int ix = int(x), iy = int(y);
    int xoffset = x - ix, yoffset = y - iy;
    float w1, w2, w3, w4;
    vec3 c1, c2, c3, c4;
    w2 = xoffset * (1 - yoffset);
    w3 = (1 - xoffset) * yoffset;
    w4 = xoffset * yoffset;
    w1 = 1 - w2 - w3 - w4;
    if (xoffset > 0.5f) {
        xoffset = ix + 1;
    } else if (xoffset < 0.5f) {
        xoffset = ix - 1;
    } else {
        xoffset = ix;
    }
    if (yoffset > 0.5f) {
        yoffset = iy + 1;
    } else if (yoffset < 0.5f) {
        yoffset = iy - 1;
    } else {
        yoffset = iy;
    }
    xoffset = xoffset < 0 ? 0 : xoffset;
    xoffset = xoffset == width ? (width - 1) : xoffset;
    yoffset = yoffset < 0 ? 0 : xoffset;
    yoffset = yoffset == height ? (height - 1) : xoffset;
    c1 = image.getV3(ix, iy);
    c2 = image.getV3(xoffset, iy);
    c3 = image.getV3(ix, yoffset);
    c4 = image.getV3(xoffset, yoffset);
    vec3 interpolatedc = w1 * c1 + w2 * c2 + w3 * c3 + w4 * c4;
    std::swap(interpolatedc[0], interpolatedc[2]);  // tgaimage use bgr which should be transfer to rgb
    return interpolatedc / 255.f;
}

float Shader::textureShadow(float x, float y) {
    int width = screen_w;
    int height = screen_h;
    if ((x < 0) || (x > width - 1) || (y < 0) || (y > height - 1)) {
        return -1;
    }
    int ix = int(x), iy = int(y);
    int xoffset = x - ix, yoffset = y - iy;
    float w1, w2, w3, w4;
    float c1, c2, c3, c4;
    w2 = xoffset * (1 - yoffset);
    w3 = (1 - xoffset) * yoffset;
    w4 = xoffset * yoffset;
    w1 = 1 - w2 - w3 - w4;
    if (xoffset > 0.5f) {
        xoffset = ix + 1;
    } else if (xoffset < 0.5f) {
        xoffset = ix - 1;
    } else {
        xoffset = ix;
    }
    if (yoffset > 0.5f) {
        yoffset = iy + 1;
    } else if (yoffset < 0.5f) {
        yoffset = iy - 1;
    } else {
        yoffset = iy;
    }
    xoffset = xoffset < 0 ? 0 : xoffset;
    xoffset = xoffset == width ? (width - 1) : xoffset;
    yoffset = yoffset < 0 ? 0 : xoffset;
    yoffset = yoffset == height ? (height - 1) : xoffset;
    c1 = zbuffer_[ix + iy * width];
    c2 = zbuffer_[xoffset + iy * width];
    c3 = zbuffer_[ix + yoffset * width];
    c4 = zbuffer_[xoffset + yoffset * width];
    float interpolatedc = w1 * c1 + w2 * c2 + w3 * c3 + w4 * c4;
    return interpolatedc;
}
