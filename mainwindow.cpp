#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "raytracer.h"
#include "main.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0, -1), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
    GetRaytracer()->SetCanvas(300,300);
    GetRaytracer()->RenderThreaded();
}

void MainWindow::paintEvent(QPaintEvent *pe)
{
//    ui->graphicsView->scene()->clear();
    ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(GetRaytracer()->GetImage()));
    ui->graphicsView->setDisabled(false);

    QMainWindow::paintEvent(pe);
}

MainWindow::~MainWindow()
{
    delete ui;
}

