#ifndef RENDERLOOP_H
#define RENDERLOOP_H
#include <QObject>

#include "BlinPhongShader.h"
#include "ImageBuffer.h"
#include "Model.h"
#include "utils.h"

class RenderLoop : public QObject {
    Q_OBJECT
public:
    RenderLoop(int w, int h);
    ~RenderLoop();
    void Resize(int w, int h);

private:
    int width_;
    int height_;
    ImageBuffer* render_buffer_;

    // model1
    Model* model1_;
    BlinPhongShader* model1shader;

    // camera
    vec3 camera_pos_ = vec3(0, 0, 2);
    vec3 camera_lookat_ = vec3(0, 0, -1);
    vec3 camera_up_ = vec3(0, 1, 0);

    // view
    float near_ = 3;
    float far_ = 1;
    float fov_ = 1.5f;
    float aspect_;
    mat4 view_matrix_;

    // illumination
    vec3 light_pos_ = vec3(2, 4, 2);
    float light_intensity_ = 10.f;

    mat4 perspective_matrix_;
    float* zbuffer_;
    void RenderDepthBuffer(std::vector<std::vector<vec3>> face, mat4 mvp);
    void BPRasterize(Model* model, BlinPhongShader* shader);
signals:
    void FrameReady(unsigned char* data);
public slots:
    void MainLoop();
};

#endif  // RENDERLOOP_H
