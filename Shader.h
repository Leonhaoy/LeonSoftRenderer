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
    vec3 unortho_tangent;
};
struct vertexDataOut {
    vec3 world_pos;
    vec4 screen_pos;
    vec3 tangent_light_dir;
    vec3 tangent_camera_dir;
};

struct fragmentDataIn {
    vec3 world_pos;
    vec4 screen_pos;
    vec3 light_dir;
    vec3 camera_dir;
    bool front_facing;
    vec2 uv;
};

class Shader {
public:
    Shader(const int& w, const int& h) : screen_w(w), screen_h(h), zbuffer_(nullptr) {}
    ~Shader() {}
    virtual void VertexShading(IN vertexDataIn& in, OUT vertexDataOut& out);
    virtual vec4 FragmentShading(IN fragmentDataIn& in);
    virtual void Init(const mat4& model, const mat4& view_projection, const vec3& camera_pos) {
        model_ = model;
        view_projection_ = view_projection;
        camera_pos_ = camera_pos;
    }
    void set_zbuffer(float* zb) { zbuffer_ = zb; }
    vec3 texture(vec2 uv, TGAImage& image);
    float textureShadow(float x, float y);

protected:
    mat4 model_;
    mat4 view_projection_;
    vec3 camera_pos_;

    int screen_w;
    int screen_h;
    float* zbuffer_;
};

#endif  // SHADER_H
