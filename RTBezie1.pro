#-------------------------------------------------
#
# Project created by QtCreator 2012-11-04T12:57:41
#
#-------------------------------------------------

QT       += core gui

TARGET = RTBezie1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    raytracer.cpp \
    camera.cpp \
    ray.cpp \
    cmesh.cpp \
    intersactioninfo.cpp \
    triangle.cpp \
    raytracerthread.cpp \
    settings.cpp \
    kdtreenode.cpp \
    AABox.cpp \
    Utils.cpp \
    light.cpp \
    lightsscene.cpp \
    lightpoint.cpp \
    shader.cpp

HEADERS  += mainwindow.h \
    raytracer.h \
    main.h \
    camera.h \
    ray.h \
    cmesh.h \
    intersactioninfo.h \
    triangle.h \
    raytracerthread.h \
    settings.h \
    kdtreenode.h \
    AABox.h \
    Utils.h \
    light.h \
    lightsscene.h \
    lightpoint.h \
    shader.h

FORMS    += mainwindow.ui
