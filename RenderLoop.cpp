#include "RenderLoop.h"

#include <windows.h>

#include "time_utils.h"

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

    // printMat4(view_matrix_);

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
    // printMat4(perspective_matrix_);
}
RenderLoop::~RenderLoop() {
    delete render_buffer_;
    delete model1_;
}
void RenderLoop::Resize(int w, int h) {
    width_ = w;
    height_ = h;
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
    model1_->transform_.position = vec3(0, 0, -1);
    std::vector<std::vector<vec3>> modelvs = model1_->get_triangle_vertexs_();
    std::vector<std::vector<vec2>> modeluvs = model1_->get_triangle_uvs_();
    std::vector<std::vector<vec3>> modelns = model1_->get_triangle_normals_();
    int trisize = model1_->get_triangles_size_();

    mat4 model1mat = model1_->GetModelMatrix();
    mat4 mvp = perspective_matrix_ * view_matrix_ * model1mat;
    // renderloop
    while (true) {
        // std::cout << "MainLoop render " << renderframeindex++ << std::endl;
        uint64_t now = get_current_ms();
        lastrendertime = now;
        render_buffer_->Resize(width_, height_);
        render_buffer_->Clear();

        vec4 white(255, 255, 255, 255);

        // draw line of the model
        for (int i = 0; i < trisize; i++) {
            for (int j = 0; j < 3; j++) {
                vec4 v0 = mvp * vec4(modelvs[i][j], 1) + vec4(1, 1, 0, 0);
                vec4 v1 = mvp * vec4(modelvs[i][(j + 1) % 3], 1) + vec4(1, 1, 0, 0);

                line(v0.x * width_ / 2, v0.y * height_ / 2, v1.x * width_ / 2, v1.y * height_ / 2, render_buffer_,
                     white);
            }
        }
        renderframeindex++;
        emit FrameReady(render_buffer_->Data());
        uint64_t sleeptime = lastrendertime + 1000 - get_current_ms();
        if (sleeptime > 0) {
            Sleep(sleeptime);
        }
    }
}
