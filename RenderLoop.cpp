#include "RenderLoop.h"

#include <windows.h>

#include "time_utils.h"

vec3 GetBarycentric(const vec2& p, const vec2& v1, const vec2& v2, const vec2& v3) {
    vec2 vp3 = p - v3;
    vec2 v13 = v1 - v3;
    vec2 v23 = v2 - v3;
    float w1 = (vp3.x * v23.y - vp3.y * v23.x) / (v13.x * v23.y - v13.y * v23.x);
    float w2 = (vp3.x * v13.y - vp3.y * v13.x) / (v23.x * v13.y - v23.y * v13.x);
    float w3 = 1 - w1 - w2;
    return vec3(w1, w2, w3);
}

void line(int x0, int y0, int x1, int y1, ImageBuffer* image, vec4 color) {
    bool transposed = false;
    if (abs(x0 - x1) < abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        transposed = true;
    }
    if (x1 < x0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0, dy = abs(y1 - y0);

    float step = abs(dy / (float)dx);
    int ystep = y0 > y1 ? -1 : 1;
    float offset = 0;
    for (float x = x0, y = y0; x <= x1; x++) {
        if (transposed)
            image->SetColorOfPixel(y, x, color);
        else
            image->SetColorOfPixel(x, y, color);
        offset += step;
        if (offset > 0.5) {
            y += ystep;
            offset--;
        }
    }
}
RenderLoop::RenderLoop(int w, int h) : width_(w), height_(h) {
    render_buffer_ = new ImageBuffer(w, h);

    vec3 norlookat = normalize(camera_lookat_);
    vec3 norup = normalize(camera_up_);
    vec3 norright = normalize(cross(norlookat, norup));
    norlookat = -norlookat;

    view_matrix_ = translate(mat4(norright.x, norup.x, norlookat.x, 0, norright.y, norup.y, norlookat.y, 0, norright.z,
                                  norup.z, norlookat.z, 0, 0, 0, 0, 1),
                             -camera_pos_);

    printMat4(view_matrix_);

    aspect_ = (float)w / h;
    /*
     *  perspective_matrix_ = MorthographicTocuboid * MperspectiveTOorthographic
     *
    float halftanfov = tan(2 / fov_);
    perspective_matrix_ = mat4(1 / (halftanfov * near_ * aspect_), 0, 0, 0, 0, 1 / (halftanfov * near_), 0, 0, 0, 0,
                               2 / (near_ - far_), 0, 0, 0, (far_ + near_) / (far_ - near_), 1) *
                          mat4(near_, 0, 0, 0, 0, near_, 0, 0, 0, 0, near_ + far_, 1, 0, 0, -near_ * far_, 0);
    perspective_matrix_ = mat4(1 / (halftanfov * aspect_), 0, 0, 0, 0, 1 / halftanfov, 0, 0, 0, 0,
                               (near_ + far_) / (near_ - far_), 1, 0, 0, 2 * near_ * far_ / (near_ - far_), 0);
    */
    perspective_matrix_ = perspective(fov_, aspect_, near_, far_);
    printMat4(perspective_matrix_);
    zbuffer_ = new float[width_ * height_];
    for (int i = 0; i < width_ * height_; ++i) {
        zbuffer_[i] = 0;
    }
}
RenderLoop::~RenderLoop() {
    delete render_buffer_;
    delete model1_;
    delete zbuffer_;
}
void RenderLoop::Resize(int w, int h) {
    width_ = w;
    height_ = h;
}

void RenderLoop::DepthTest(std::vector<std::vector<vec3>> face, mat4 mvp) {
    int size = face.size();
    for (int i = 0; i < size; i++) {
        vec4 v0 = mvp * vec4(face[i][0], 1);
        vec4 v1 = mvp * vec4(face[i][1], 1);
        vec4 v2 = mvp * vec4(face[i][2], 1);
        v0 = v0 / v0.w + vec4(1, 1, 1, 0);
        v1 = v1 / v1.w + vec4(1, 1, 1, 0);
        v2 = v2 / v2.w + vec4(1, 1, 1, 0);
        v0 = vec4(v0.x * width_ / 2, v0.y * height_ / 2, v0.z / 2, 1);
        v1 = vec4(v1.x * width_ / 2, v1.y * height_ / 2, v1.z / 2, 1);
        v2 = vec4(v2.x * width_ / 2, v2.y * height_ / 2, v2.z / 2, 1);
        if (dot(camera_lookat_, cross(vec3(v1 - v0), vec3(v2 - v1))) > 0)
            continue;
        int boundingbox_minx = min(min(v0.x, v1.x), v2.x);
        int boundingbox_maxx = max(max(v0.x, v1.x), v2.x);
        int boundingbox_miny = min(min(v0.y, v1.y), v2.y);
        int boundingbox_maxy = max(max(v0.y, v1.y), v2.y);

        for (int x = boundingbox_minx; x <= boundingbox_maxx; x++) {
            for (int y = boundingbox_miny; y <= boundingbox_maxy; y++) {
                if (x < 0 || x >= width_ || y < 0 || y >= height_)
                    continue;
                vec3 barycentric = GetBarycentric(vec2(x, y), v0, v1, v2);
                if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0)
                    continue;
                float z = barycentric.x * v0.z + barycentric.y * v1.z + barycentric.z * v2.z;
                float* zb = zbuffer_ + y * width_ + x;

                if (z > *zb) {
                    *zb = z;
                }
            }
        }
        // render zbuffer
        for (int i = 0; i < width_; i++) {
            for (int j = 0; j < height_; j++) {
                render_buffer_->SetColorOfPixel(i, j, vec4(vec3(1) * (zbuffer_[j * width_ + i] * 255.f), 255));
            }
        }
    }
}
void RenderLoop::MainLoop() {
    TCHAR pathbuffer[MAX_PATH];
    if (!GetCurrentDirectory(MAX_PATH, pathbuffer)) {
        std::cerr << "Can not get current path" << std::endl;
        return;
    }
    std::wstring tmp(&pathbuffer[0]);
    std::string workpath(tmp.begin(), tmp.end());
    std::cout << "Current path:" << workpath << std::endl;
    int renderframeindex = 0;
    uint64_t lastrendertime = 0;
    std::string objpath = workpath + "/obj/african_head.obj";
    model1_ = new Model();
    model1_->ReadObjFile(objpath.c_str());
    model1_->transform_.position = vec3(0, 0, 0);
    model1_->transform_.rotation = vec3(0, 0, 0);
    model1_->transform_.scale = vec3(1) * 0.6f;
    std::vector<std::vector<vec3>> modelvs = model1_->get_triangle_vertexs_();
    std::vector<std::vector<vec2>> modeluvs = model1_->get_triangle_uvs_();
    std::vector<std::vector<vec3>> modelns = model1_->get_triangle_normals_();

    mat4 model1mat = model1_->GetModelMatrix();
    mat4 mvp = perspective_matrix_ * view_matrix_ * model1mat;
    printMat4(mvp);
    // renderloop
    while (true) {
        // std::cout << "MainLoop render " << renderframeindex++ << std::endl;
        uint64_t now = get_current_ms();
        lastrendertime = now;
        render_buffer_->Resize(width_, height_);
        render_buffer_->Clear();

        vec4 white(255, 255, 255, 255);

        /*
        // draw line of the model
        for (int i = 0; i < trisize; i++) {
            for (int j = 0; j < 3; j++) {
                vec4 v0 = mvp * vec4(modelvs[i][j], 1);
                vec4 v1 = mvp * vec4(modelvs[i][(j + 1) % 3], 1);
                v0 = v0 / v0.w + vec4(1, 1, 0, 0);
                v1 = v1 / v1.w + vec4(1, 1, 0, 0);
                line(v0.x * width_ / 2, v0.y * height_ / 2, v1.x * width_ / 2, v1.y * height_ / 2, render_buffer_,
                     white);
            }
        }
        */
        DepthTest(modelvs, mvp);

        renderframeindex++;
        emit FrameReady(render_buffer_->Data());
        uint64_t sleeptime = lastrendertime + 100000 - get_current_ms();
        if (sleeptime > 0) {
            Sleep(sleeptime);
        }
    }
}
