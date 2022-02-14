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
    model1 = new Model();
    model1->ReadObjFile(objpath.c_str());
    std::vector<std::vector<vec3>> modelvs = model1->get_triangle_vertexs_();
    std::vector<std::vector<vec2>> modeluvs = model1->get_triangle_uvs_();
    std::vector<std::vector<vec3>> modelns = model1->get_triangle_normals_();
    int trisize = model1->get_triangles_size_();

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
                vec3 v0 = modelvs[i][j] + vec3(1, 1, 0);
                vec3 v1 = modelvs[i][(j + 1) % 3] + vec3(1, 1, 0);

                line(v0.x * width_ / 2, v0.y * height_ / 2, v1.x * width_ / 2, v1.y * height_ / 2, render_buffer_,
                     white);
            }
        }

        emit FrameReady(render_buffer_->Data());
        uint64_t sleeptime = lastrendertime + 1000 - get_current_ms();
        if (sleeptime > 0) {
            Sleep(sleeptime);
        }
    }
}
