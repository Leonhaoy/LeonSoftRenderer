#ifndef RENDERLOOP_H
#define RENDERLOOP_H
#include <QObject>

#include "BlinPhongShader.h"
#include "ImageBuffer.h"
#include "Model.h"
#include "ShadowShader.h"
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

    // camera
    //    vec3 camera_pos_ = vec3(0, 0, 2);
    //    vec3 camera_lookat_ = vec3(0, 0, -1);
    //    vec3 camera_up_ = vec3(0, 1, 0);
    vec3 camera_pos_ = vec3(0, 2, 2);
    vec3 camera_lookat_ = vec3(0, -4, -5);
    vec3 camera_up_ = vec3(0, 1, 0);

    // view
    float near_ = 10;
    float far_ = 1;
    float fov_ = 1.3f;
    float aspect_;
    mat4 view_matrix_;

    // illumination
    vec3 light_pos_ = vec3(2, 2, 2);
    vec3 light_lookat_ = vec3(-2, -4, -2);
    float light_intensity_ = 10.f;
    mat4 light_matrix_;

    mat4 perspective_matrix_;
    float* zbuffer_;
    float* shadow_map_;
    void BPPipeLine(Model* model, BlinPhongShader* shader);
    void ShadowPass(Model* model, ShadowShader* shader);
signals:
    void FrameReady(unsigned char* data);
public slots:
    void MainLoop();
};

#endif  // RENDERLOOP_H
