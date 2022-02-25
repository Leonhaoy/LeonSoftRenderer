#ifndef SHADOWSHADER_H
#define SHADOWSHADER_H
#include "Shader.h"
class ShadowShader : public Shader {
public:
    ShadowShader(const int& w, const int& h) : Shader(w, h) {}
    ~ShadowShader() {}
    void VertexShading(IN vertexDataIn& in, OUT vertexDataOut& out) override;
    vec4 FragmentShading(IN fragmentDataIn& in) override;
    void Init(const mat4& model, const mat4& view_projection, const vec3& camera_pos) {
        Shader::Init(model, view_projection, camera_pos);
    }
};
#endif  // SHADOWSHADER_H
