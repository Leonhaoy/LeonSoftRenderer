#include "Model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int Model::ReadObjFile(const char* filename) {
    std::ifstream in;
    std::cout << "Try to read model from : " << std::string(filename) << std::endl;
    in.open(filename, std::ifstream::in);
    if (in.fail()) {
        std::cerr << "Can not open the model file!" << std::endl;
        return -1;
    }
    std::string line;
    char trash;
    std::vector<vec3> ver;
    std::vector<vec2> tex;
    std::vector<vec3> norm;
    // obj file start from 1,fill the empty data
    vertexs_.push_back(vec3(0, 0, 0));
    uvs_.push_back(vec2(0, 0));
    normals_.push_back(vec3(0, 0, 0));
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for (int i = 0; i < 3; i++)
                iss >> v[i];
            vertexs_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            ver.clear();
            tex.clear();
            norm.clear();
            size_t texIdx, vertexIdx, normalIdx;
            iss >> trash;
            while (iss >> vertexIdx >> trash >> texIdx >> trash >> normalIdx) {
                ver.push_back(vertexs_[vertexIdx]);
                tex.push_back(uvs_[texIdx]);
                norm.push_back(normals_[normalIdx]);
            }
            triangle_vertexs_.push_back(ver);
            triangle_uvs_.push_back(tex);
            triangle_normals_.push_back(norm);

        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            vec2 v;
            for (int i = 0; i < 2; i++)
                iss >> v[i];
            uvs_.push_back(v);
            iss >> v[0];
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            vec3 v;
            for (int i = 0; i < 3; i++)
                iss >> v[i];
            normals_.push_back(v);
        }
    }
    triangles_size_ = triangle_vertexs_.size();
    std::cout << "Model file read finished:" << std::endl
              << "vertex: " << vertexs_.size() << std::endl
              << "face: " << triangle_vertexs_.size() << std::endl
              << "uv: " << uvs_.size() << std::endl
              << "normal: " << normals_.size() << std::endl;
    return 0;
}
