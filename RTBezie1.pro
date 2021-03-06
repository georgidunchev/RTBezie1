#-------------------------------------------------
#
# Project created by QtCreator 2012-11-04T12:57:41
#
#-------------------------------------------------

QT       += core gui

TARGET = RTBezie1
TEMPLATE = app

QT += widgets

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
    shader.cpp \
    color.cpp \
    primitive.cpp \
    vertex.cpp \
    cview.cpp \
    SubTriangle.cpp \
    bezierpatch.cpp \
    vector3df.cpp

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
    shader.h \
    color.h \
    primitive.h \
    vertex.h \
    cview.h \
    SubTriangle.h \
    bezierpatch.h \
    vector3df.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    ToDo.txt \
    Notes.txt

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
#QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O2
