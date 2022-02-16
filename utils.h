#ifndef UTILS_H
#define UTILS_H
#include <time.h>

#include <QDebug>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iomanip>
#include <iostream>
using namespace glm;
const float PI = 3.1415927f;
inline void printMat2(const mat2& mat) {
    std::cout << "[\n";
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            std::cout << mat[j][i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "]\n";
}

inline void printMat4(const mat4& mat) {
    std::cout << "[\n";
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << mat[j][i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "]\n";
}

inline void printVec3(const vec3& vec) {
    std::cout << "[ " << vec[0] << " " << vec[1] << " " << vec[2] << "]" << std::endl;
}

inline void printVec4(const vec4& vec) {
    std::cout << "[ " << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3] << "]" << std::endl;
}

#endif  // UTILS_H
