#include "BlinPhongShader.h"

void BlinPhongShader::VertexShading(const vertexDataIn& in, vertexDataOut& out) {
    out.world_pos = model_ * in.vertex_pos;
    vec4 pos = view_projection_ * out.world_pos;
    pos = pos / pos.w + vec4(1, 1, 0, 0);
    out.screen_pos = vec4(pos.x * screen_w / 2, pos.y * screen_h / 2, pos.z, 1);
}

vec4 BlinPhongShader::FragmentShading(const fragmentDataIn in) {
    vec3 color(1);
    vec3 pos = in.world_pos;
    float power = light_power_ / pow(distance(light_pos_, pos), 2);
    vec3 diffuse = k.x * max(0.f, dot(normalize(light_pos_ - pos), in.normal)) * power * color;
    vec3 specular = k.y * (float)pow(max(0.f, dot(normalize(camera_pos_ - pos), in.normal)), 80) * power * vec4(1);
    vec3 ambient = k.z * color;
    return vec4(diffuse + specular + ambient, 1);
}
