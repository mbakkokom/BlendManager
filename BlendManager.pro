#-------------------------------------------------
#
# Project created by QtCreator 2016-04-01T10:02:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BlendManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        blend_exec.cpp \
    blend_json.cpp \
    blend_install.cpp

HEADERS  += mainwindow.h \
    blend_datastruct.h \
    blend_exec.h \
    blend_json.h \
    blend_install.h \
    main.h

#FORMS    += mainwindow.ui

RESOURCES += resources.qrc

CONFIG += c++11 static

OTHER_FILES += \
    settings.json \
    versions.json
