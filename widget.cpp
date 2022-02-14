#include "widget.h"

#include <iostream>

#include "ui_widget.h"

Widget::Widget(QWidget* parent) : QWidget(parent), ui(new Ui::Widget) {
    curFrame = nullptr;
    ui->setupUi(this);
}

void Widget::renderFrame(unsigned char* data) {
    if (curFrame) {
        delete curFrame;
    }
    curFrame = new QImage(data, width(), height(), QImage::Format_RGBA8888);
    update();
}

void Widget::paintEvent(QPaintEvent*) {
    std::cout << "paintEvent call" << std::endl;
    if (curFrame) {
        QPainter painter(this);
        painter.drawImage(0, 0, curFrame->mirrored());
    }
}

Widget::~Widget() { delete ui; }
