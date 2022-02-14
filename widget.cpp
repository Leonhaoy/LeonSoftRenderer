#include "widget.h"

#include <iostream>

#include "ui_widget.h"

Widget::Widget(QWidget* parent) :
    QWidget(parent), ui_(new Ui::Widget), cur_frame_(nullptr), loop_thread_(nullptr), loop_(nullptr) {
    cur_frame_ = nullptr;
    ui_->setupUi(this);
}
Widget::~Widget() {
    delete ui_;
    delete cur_frame_;
    delete loop_thread_;
    delete loop_;
}
void Widget::Init() {
    loop_ = new RenderLoop(width(), height());
    loop_thread_ = new QThread(this);
    loop_->moveToThread(loop_thread_);

    connect(loop_thread_, &QThread::started, loop_, &RenderLoop::MainLoop);
    connect(loop_, &RenderLoop::FrameReady, this, &Widget::RenderFrame);

    loop_thread_->start();
}
void Widget::RenderFrame(unsigned char* data) {
    if (cur_frame_) {
        delete cur_frame_;
    }
    cur_frame_ = new QImage(data, width(), height(), QImage::Format_RGBA8888);
    update();
}

void Widget::paintEvent(QPaintEvent*) {
    // std::cout << "paintEvent call" << std::endl;
    if (cur_frame_) {
        QPainter painter(this);
        painter.drawImage(0, 0, cur_frame_->mirrored());
    }
}
void Widget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    std::cout << "resize" << std::endl;
    if (loop_) {
        loop_->Resize(width(), height());
    }
}
