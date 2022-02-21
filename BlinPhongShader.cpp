#include "BlinPhongShader.h"

void BlinPhongShader::VertexShading(IN vertexDataIn& in, OUT vertexDataOut& out) {
    vec4 wp = model_ * in.vertex_pos;
    out.world_pos = wp;
    vec4 pos = view_projection_ * wp;
    pos = pos / pos.w + vec4(1, 1, 0, 0);
    out.screen_pos = vec4(pos.x * screen_w / 2, pos.y * screen_h / 2, pos.z, 1);
    //基于该点的法向正交化切线空间并转化到世界坐标系，求出TBN，用于转换光照和视线向量
    vec3 tangent = model_ * vec4(normalize(in.unortho_tangent - dot(in.unortho_tangent, in.normal)), 0);
    vec3 normal = model_ * vec4(in.normal, 0);
    vec3 bitangent = model_ * vec4(normalize(cross(normal, tangent)), 0);
    mat3 tbn = transpose(mat3(tangent, bitangent, normal));
    out.tangent_camera_dir = tbn * (camera_pos_ - out.world_pos);
    out.tangent_light_dir = tbn * (light_pos_ - out.world_pos);
}

vec4 BlinPhongShader::FragmentShading(IN fragmentDataIn& in) {
    vec3 pos = in.world_pos;
    float power = light_power_ / pow(distance(light_pos_, pos), 2);
    vec3 difftex = texture(in.uv, diffuse_img_);
    vec3 normal = normalize(texture(in.uv, normal_img_) * 2.f - vec3(1));
    vec3 diffuse = k.x * max(0.f, dot(normalize(in.light_dir), normal)) * power * difftex;
    vec3 specular = k.y * (float)pow(max(0.f, dot(normalize(in.camera_dir), normal)), 16) * power * difftex;
    vec3 ambient = k.z * vec3(1);
    return vec4(diffuse + specular + ambient, 1);
}
