#ifndef __MODEL_H__
#define __MODEL_H__
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
using namespace glm;
//目前仅支持读取obj模型
struct Transform {
    Transform() {}
    Transform(vec3 p, vec3 r) : position(p), rotation(r) {}
    vec3 position;
    vec3 rotation;
};
class Model {
private:
    //对于本渲染器直接存储vec而不是索引，通过牺牲存储空间获取访问效率的提升（避免二次索引访问降低cache hit）
    std::vector<std::vector<vec3>> triangle_vertexs_;
    std::vector<std::vector<vec2>> triangle_uvs_;
    std::vector<std::vector<vec3>> triangle_normals_;
    std::vector<vec3> vertexs_;
    std::vector<vec2> uvs_;
    std::vector<vec3> normals_;
    size_t triangles_size_;

public:
    Model() {}
    int ReadObjFile(const char* filename);
    ~Model() {}
    std::vector<std::vector<vec3>> get_triangle_vertexs_() { return triangle_vertexs_; }
    std::vector<std::vector<vec2>> get_triangle_uvs_() { return triangle_uvs_; }
    std::vector<std::vector<vec3>> get_triangle_normals_() { return triangle_normals_; }
    size_t get_triangles_size_() { return triangles_size_; }
    mat4 GetModelMatrix();
    Transform transform_;
};

#endif  //__MODEL_H__
