#ifndef SHADER_H
#define SHADER_H
#define IN const
#define OUT
#include <Model.h>

#include "tgaimage.h"
#include "utils.h"
struct vertexDataIn {
    vec4 vertex_pos;
    vec3 normal;
};
struct vertexDataOut {
    vec4 world_pos;
    vec4 screen_pos;
};

struct fragmentDataIn {
    vec4 world_pos;
    vec4 screen_pos;
    vec3 normal;
    bool front_facing;
    vec2 uv;
};

class Shader {
public:
    Shader(const int& w, const int& h) : screen_w(w), screen_h(h) {}
    ~Shader() {}
    virtual void VertexShading(IN vertexDataIn& in, OUT vertexDataOut& out);
    virtual vec4 FragmentShading(IN fragmentDataIn in);
    virtual void Init(const mat4& model, const mat4& view_projection, const vec3& camera_pos) {
        model_ = model;
        view_projection_ = view_projection;
        camera_pos_ = camera_pos;
    }
    void set_zbuffer(float* zb) { zbuffer_ = zb; }
    vec3 texture(vec2 uv, TGAImage& image);

protected:
    mat4 model_;
    mat4 view_projection_;
    vec3 camera_pos_;

    float* zbuffer_;
    int screen_w;
    int screen_h;
};

#endif  // SHADER_H
