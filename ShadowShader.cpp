#include "ShadowShader.h"

void ShadowShader::VertexShading(IN vertexDataIn& in, OUT vertexDataOut& out) {
    vec4 wp = model_ * in.vertex_pos;
    vec4 pos = view_projection_ * wp;

    out.screen_pos = vec4((pos.x / pos.w + 1) * screen_w / 2, (pos.y / pos.w + 1) * screen_h / 2, pos.z / pos.w, pos.w);
}

vec4 ShadowShader::FragmentShading(IN fragmentDataIn& in) { return vec4(1); }
