#ifndef BLINPHONGSHADER_H
#define BLINPHONGSHADER_H

#include "Shader.h"
class BlinPhongShader : public Shader {
public:
    BlinPhongShader(const int& w, const int& h) : Shader(w, h) {}
    ~BlinPhongShader() {}
    void VertexShading(IN vertexDataIn& in, OUT vertexDataOut& out) override;
    vec4 FragmentShading(IN fragmentDataIn& in) override;
    void Init(const mat4& model, const mat4& view_projection, const vec3& camera_pos, const vec3& light_pos,
              const float& light_power) {
        Shader::Init(model, view_projection, camera_pos);
        light_pos_ = light_pos;
        light_power_ = light_power;
    }
    void set_diffuse_img_(TGAImage& image) { diffuse_img_ = image; }
    void set_normal_img_(TGAImage& image) { normal_img_ = image; }

private:
    vec3 light_pos_;
    float light_power_;
    TGAImage diffuse_img_;
    TGAImage normal_img_;
    vec3 k = vec3(1.f, 0.2f, 0.2f);
};

#endif  // BLINPHONGSHADER_H
