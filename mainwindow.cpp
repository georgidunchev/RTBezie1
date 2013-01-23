#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "raytracer.h"
#include "main.h"
#include <cmesh.h>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
//    , progress("Rendering", "Abort")
{
    ui->setupUi(this);
    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0, -1), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
    GetRaytracer()->SetCanvas(300,300);

    progress.setLabelText("Rendering");
    progress.setWindowModality(Qt::WindowModal);
}

void MainWindow::paintEvent(QPaintEvent *pe)
{
//    ui->graphicsView->scene()->clear();

    ui->graphicsView->setDisabled(false);

    QMainWindow::paintEvent(pe);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openMeshButton_clicked()
{
    QString strFileName;
    strFileName = QFileDialog::getOpenFileName(this,
	tr("Open Model"), "", tr("Image Files (*.obj)"));

    GetRaytracer()->LoadNewMesh(strFileName);

    emit EnableRenderButton(true);
}

void MainWindow::on_StartRender_clicked()
{
    progress.setMaximum( GetRaytracer()->GetBucketsCount() );

    QObject::connect(GetRaytracer(), SIGNAL(sigBucketDone(int)), &progress, SLOT(setValue(int)));
    QObject::connect(GetRaytracer(), SIGNAL(sigThreadsFinished()), this, SLOT(slotRenderFinished()));

    GetRaytracer()->GetTimer().start();

    GetRaytracer()->RenderThreaded();
}

void MainWindow::slotRenderFinished()
{
    progress.reset();

    ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(GetRaytracer()->GetImage()));

    if (GetRaytracer()->GetTimer().isValid())
    {
	qDebug()<<"Render Finished in"<<GetRaytracer()->GetTimer().elapsed()/1000.f;
	GetRaytracer()->GetTimer().invalidate();
    }
}
