#ifndef RENDERLOOP_H
#define RENDERLOOP_H
#include <QObject>

#include "ImageBuffer.h"
#include "Model.h"
class RenderLoop : public QObject {
    Q_OBJECT
public:
    RenderLoop(int w, int h) : width_(w), height_(h) { render_buffer_ = new ImageBuffer(w, h); }
    ~RenderLoop() { delete render_buffer_; }
    void Resize(int w, int h);

private:
    int width_;
    int height_;
    ImageBuffer* render_buffer_;
    Model* model1;
signals:
    void FrameReady(unsigned char* data);
public slots:
    void MainLoop();
};

#endif  // RENDERLOOP_H
