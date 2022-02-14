#ifndef WIDGET_H
#define WIDGET_H

#include <QPainter>
#include <QThread>
#include <QWidget>

#include "RenderLoop.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT
public:
    Widget(QWidget* parent = nullptr);
    ~Widget();
    void RenderFrame(unsigned char* data);
    void Init();

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::Widget* ui_;
    QImage* cur_frame_;
    QThread* loop_thread_;
    RenderLoop* loop_;
};
#endif  // WIDGET_H
