#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "raytracer.h"
#include "main.h"
#include <cmesh.h>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0, -1), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
    GetRaytracer()->SetCanvas(300,300);
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


    GetRaytracer()->GetMesh().Load(strFileName);

    emit EnableRenderButton(true);
}

void MainWindow::on_StartRender_clicked()
{
    int numFiles = GetRaytracer()->GetBucketsCount();
    progress = new QProgressDialog("Subdividing", "Abort", 0, numFiles);
    progress->setWindowModality(Qt::WindowModal);

    QObject::connect(GetRaytracer(), SIGNAL(sigBucketDone(int)), progress, SLOT(setValue(int)));
    QObject::connect(GetRaytracer(), SIGNAL(sigThreadsFinished()), this, SLOT(slotRenderFinished()));

    GetRaytracer()->GetTimer().start();

    GetRaytracer()->RenderThreaded();


}

void MainWindow::slotRenderFinished()
{
    if (progress)
    {
	progress->reset();
	delete progress;
    }
    ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(GetRaytracer()->GetImage()));

    if (GetRaytracer()->GetTimer().isValid())
    {
	qDebug()<<"Render Finished in"<<GetRaytracer()->GetTimer().elapsed()/1000.f;
	GetRaytracer()->GetTimer().invalidate();
    }
}
