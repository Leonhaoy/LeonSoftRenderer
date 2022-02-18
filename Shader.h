#ifndef SHADER_H
#define SHADER_H
#define IN const
#define OUT
#include <Model.h>

#include "tgaimage.h"
#include "utils.h"
struct vertexData {
    vec4 vertex_pos;
    vec4 normal;
};
struct fragmentData {
    vec4 world_pos;
    vec4 screen_pos;
    bool front_facing;
    vec2 uv;
};

class Shader {
public:
    virtual void VertexShading(IN vertexData& in, OUT fragmentData& out);
    virtual vec4 FragmentShading(IN fragmentData);
    vec3 texture(vec2 uv, TGAImage& image);

private:
    mat4 model_;
    mat4 view_;
    mat4 projection_;
    vec4 camera_pos_;

    float* zBuffer;
    int screen_h;
    int screen_w;
};

#endif  // SHADER_H
