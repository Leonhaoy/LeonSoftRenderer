#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "ImageBuffer.h"
#include "widget.h"

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
    int width = 512, height = 512;

    w.resize(width, height);
    w.show();
    w.Init();

    return a.exec();
}
