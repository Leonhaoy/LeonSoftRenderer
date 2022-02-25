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

    //    vec3 norlookat = normalize(camera_lookat_);
    //    vec3 norright = normalize(cross(norlookat, camera_up_));
    //    camera_up_ = cross(norright, norlookat);
    //    vec3 norup = normalize(camera_up_);
    //    norlookat = -norlookat;

    //    view_matrix_ = translate(mat4(norright.x, norup.x, norlookat.x, 0, norright.y, norup.y, norlookat.y, 0,
    //    norright.z,
    //                                  norup.z, norlookat.z, 0, 0, 0, 0, 1),
    //                             -camera_pos_);
    view_matrix_ = lookAt(camera_pos_, camera_lookat_ + camera_pos_, camera_up_);

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

    light_matrix_ = lookAt(light_pos_, light_lookat_ + light_pos_, camera_up_);
    zbuffer_ = new float[width_ * height_];
    for (int i = 0; i < width_ * height_; ++i) {
        zbuffer_[i] = -1;
    }
    shadow_map_ = new float[width_ * height_];
    for (int i = 0; i < width_ * height_; ++i) {
        shadow_map_[i] = -1;
    }
}
RenderLoop::~RenderLoop() {
    delete render_buffer_;
    delete zbuffer_;
}
void RenderLoop::Resize(int w, int h) {
    width_ = w;
    height_ = h;
}

void RenderLoop::ShadowPass(Model* model, ShadowShader* shader) {
    mat4 modelmax = model->GetModelMatrix();
    std::vector<std::vector<vec3>> vertexs = model->get_triangle_vertexs_();
    int size = model->get_triangles_size_();
    mat4 vp = perspective_matrix_ * light_matrix_;
    shader->Init(modelmax, vp, light_pos_);

    for (int i = 0; i < size; i++) {
        vertexDataIn vin[3];
        vertexDataOut vout[3];
        for (int j = 0; j < 3; j++) {
            vin[j].vertex_pos = vec4(vertexs[i][j], 1);
            shader->VertexShading(vin[j], vout[j]);
        }
        vec4 v0(vout[0].screen_pos), v1(vout[1].screen_pos), v2(vout[2].screen_pos);

        // back face culling
        if (dot(light_lookat_, cross(vec3(v1 - v0), vec3(v2 - v1))) > 0)
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
                // only rasterize pixel inside the triangle
                if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0)
                    continue;

                float z = barycentric.x * v0.z + barycentric.y * v1.z + barycentric.z * v2.z;
                float* zb = shadow_map_ + y * width_ + x;
                if (z > *zb) {
                    *zb = z;
                    // render_buffer_->SetColorOfPixel(x, y, vec4((z + 1.f) * 2.f * vec3(255.f), 255));
                }
            }
        }
    }
}

void RenderLoop::BPPipeLine(Model* model, BlinPhongShader* shader) {
    mat4 modelmax = model->GetModelMatrix();
    std::vector<std::vector<vec3>> vertexs = model->get_triangle_vertexs_();
    std::vector<std::vector<vec2>> uvs = model->get_triangle_uvs_();
    std::vector<std::vector<vec3>> normals = model->get_triangle_normals_();
    int size = model->get_triangles_size_();
    mat4 vp = perspective_matrix_ * view_matrix_;
    shader->Init(modelmax, vp, camera_pos_, light_pos_, light_intensity_);
    mat4 light = perspective_matrix_ * light_matrix_;
    shader->EnableShadowMap(shadow_map_, light);
    for (int i = 0; i < size; i++) {
        vertexDataIn vin[3];
        vertexDataOut vout[3];
        // 计算出片元对应切线空间未正交化的tangent轴单位向量，传入顶点着色器中计算正交化后的tbn矩阵，并将光照注视方向转换到该坐标系下
        vec3 E1 = vertexs[i][1] - vertexs[i][0];
        vec3 E2 = vertexs[i][2] - vertexs[i][0];
        vec2 deltaUV1 = uvs[i][1] - uvs[i][0];
        vec2 deltaUV2 = uvs[i][2] - uvs[i][0];
        vec3 unortho_tangent =
            (deltaUV2.y * E1 - deltaUV1.y * E2) * (1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y));
        for (int j = 0; j < 3; j++) {
            vin[j].vertex_pos = vec4(vertexs[i][j], 1);
            vin[j].normal = normals[i][j];
            vin[j].unortho_tangent = unortho_tangent;
            shader->VertexShading(vin[j], vout[j]);
        }
        vec4 v0(vout[0].screen_pos), v1(vout[1].screen_pos), v2(vout[2].screen_pos);

        // back face culling
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
                // only rasterize pixel inside the triangle
                if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0)
                    continue;
                float w = barycentric.x * 1 / v0.w + barycentric.y * 1 / v1.w + barycentric.z * 1 / v2.w;
                w = 1 / w;
                vec3 fix_barycentrix =
                    vec3(w / v0.w * barycentric.x, w / v1.w * barycentric.y, w / v2.w * barycentric.z);

                float z = fix_barycentrix.x * v0.z + fix_barycentrix.y * v1.z + fix_barycentrix.z * v2.z;

                // depth test
                float* zb = zbuffer_ + y * width_ + x;
                if (z > *zb) {
                    *zb = z;
                    fragmentDataIn pixel;
                    pixel.world_pos = fix_barycentrix.x * vout[0].world_pos + fix_barycentrix.y * vout[1].world_pos +
                                      fix_barycentrix.z * vout[2].world_pos;
                    pixel.screen_pos = vec4(x, y, z, 1);
                    pixel.light_dir = fix_barycentrix.x * vout[0].tangent_light_dir +
                                      fix_barycentrix.y * vout[1].tangent_light_dir +
                                      fix_barycentrix.z * vout[2].tangent_light_dir;
                    pixel.camera_dir = fix_barycentrix.x * vout[0].tangent_camera_dir +
                                       fix_barycentrix.y * vout[1].tangent_camera_dir +
                                       fix_barycentrix.z * vout[2].tangent_camera_dir;

                    pixel.front_facing = true;
                    pixel.uv =
                        fix_barycentrix.x * uvs[i][0] + fix_barycentrix.y * uvs[i][1] + fix_barycentrix.z * uvs[i][2];
                    render_buffer_->SetColorOfPixel(x, y, shader->FragmentShading(pixel) * 255.f);
                }
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

    ShadowShader* shadowShader = new ShadowShader(width_, height_);
    shadowShader->set_zbuffer(shadow_map_);

    // model1
    std::string objpath = workpath + "/obj/african_head.obj";
    Model* model1 = new Model();
    model1->ReadObjFile(objpath.c_str());
    model1->transform_.position = vec3(0, 0, 0);
    model1->transform_.rotation = vec3(0, 0, 0);
    model1->transform_.scale = vec3(1) * 1.f;
    std::string diffusepath = workpath + "/image/african_head_diffuse.tga";
    std::string normalpath = workpath + "/image/african_head_nm_tangent.tga";
    TGAImage diffuseimage1, normalimage1;
    if (!diffuseimage1.read_tga_file(diffusepath.c_str())) {
        std::cerr << "Read diffuseimage file failed!" << std::endl;
    }
    if (!normalimage1.read_tga_file(normalpath.c_str())) {
        std::cerr << "Read normalpath file failed!" << std::endl;
    }
    BlinPhongShader* model1shader = new BlinPhongShader(width_, height_);
    model1shader->set_diffuse_img_(diffuseimage1);
    model1shader->set_normal_img_(normalimage1);

    // model2
    objpath = workpath + "/obj/floor.obj";
    Model* model2 = new Model();
    model2->ReadObjFile(objpath.c_str());
    model2->transform_.position = vec3(0, 0.6f, 0);
    model2->transform_.rotation = vec3(0, 0, 0);
    model2->transform_.scale = vec3(1) * 2.f;
    diffusepath = workpath + "/image/floor_diffuse.tga";
    normalpath = workpath + "/image/floor_nm_tangent.tga";
    TGAImage diffuseimage2, normalimage2;
    if (!diffuseimage2.read_tga_file(diffusepath.c_str())) {
        std::cerr << "Read diffuseimage file failed!" << std::endl;
    }
    if (!normalimage2.read_tga_file(normalpath.c_str())) {
        std::cerr << "Read normalpath file failed!" << std::endl;
    }
    BlinPhongShader* model2shader = new BlinPhongShader(width_, height_);
    model2shader->set_diffuse_img_(diffuseimage2);
    model2shader->set_normal_img_(normalimage2);

    // renderloop
    while (true) {
        //  std::cout << "MainLoop render " << renderframeindex++ << std::endl;
        uint64_t now = get_current_ms();
        lastrendertime = now;
        render_buffer_->Resize(width_, height_);
        render_buffer_->Clear();

        // RenderDepthBuffer(model1->get_triangle_vertexs_(),
        //                 perspective_matrix_ * view_matrix_ * model1->GetModelMatrix());
        ShadowPass(model1, shadowShader);
        ShadowPass(model2, shadowShader);
        BPPipeLine(model1, model1shader);
        BPPipeLine(model2, model2shader);
        std::cout << "render finish!" << std::endl;

        renderframeindex++;
        emit FrameReady(render_buffer_->Data());
        uint64_t sleeptime = lastrendertime + 100000 - get_current_ms();
        if (sleeptime > 0) {
            Sleep(sleeptime);
        }
    }
    delete model2;
    delete model1;
    delete model1shader;
    delete model2shader;
}
