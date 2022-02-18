QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Model.cpp \
    RenderLoop.cpp \
    Shader.cpp \
    main.cpp \
    tgaimage.cpp \
    widget.cpp

INCLUDEPATH += glm

HEADERS += \
    ImageBuffer.h \
    Model.h \
    RenderLoop.h \
    Shader.h \
    tgaimage.h \
    time_utils.h \
    utils.h \
    widget.h

FORMS += \
    widget.ui

TRANSLATIONS += \
    LeonSoftRender_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
