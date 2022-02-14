#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "ImageBuffer.h"
#include "widget.h"

void line(const int& x0, const int& y0, const int& x1, const int& y1, ImageBuffer* image, vec4 color) {
    for (int i = x0; i < x1; i++) {
        image->SetColorOfPixel(i, int(y0 + ((float)i - x0) / (x1 - x0) * (y1 - y0)), color);
    }
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "LeonSoftRender_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Widget w;
    w.resize(256, 256);
    w.show();
    ImageBuffer image(256, 256);
    image.Clear();
    vec4 white(255, 255, 255, 255);

    line(50, 50, 100, 100, &image, white);
    w.renderFrame(image.Data());

    return a.exec();
}
