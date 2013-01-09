#include <QtGui/QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "mainwindow.h"
#include "main.h"
#include "raytracer.h"

RayTracer * raytracer_instace;

void myMessageOutput(QtMsgType type, const char *msg)
{
    const char symbols[] = { 'I', 'E', '!', 'X' };
    QString output = QString("[%1] %2").arg( symbols[type] ).arg( msg );
    std::cerr << output.toStdString() << std::endl;
    if( type == QtFatalMsg ) abort();

//    switch (type) {
//    case QtDebugMsg:
//	std::cerr << "Debug: %s\n", msg);
//	break;
//    case QtWarningMsg:
//	fprintf(stderr, "Warning: %s\n", msg);
//	break;
//    case QtCriticalMsg:
//	fprintf(stderr, "Critical: %s\n", msg);
//	break;
//    case QtFatalMsg:
//	fprintf(stderr, "Fatal: %s\n", msg);
//	abort();
//    }123
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);

    raytracer_instace = new RayTracer();

    MainWindow w;
    w.show();
    
    return a.exec();
}

RayTracer* GetRaytracer()
{
    return raytracer_instace;
}
