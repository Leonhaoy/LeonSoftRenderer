#ifndef WIDGET_H
#define WIDGET_H

#include <QPainter>
#include <QThread>
#include <QWidget>

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
    void renderFrame(unsigned char* data);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    Ui::Widget* ui;
    QImage* curFrame;
};
#endif  // WIDGET_H