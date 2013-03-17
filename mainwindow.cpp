#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "raytracer.h"
#include "main.h"
#include <cmesh.h>
#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>
#include <settings.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
//    , progress("Rendering", "Abort")
{
    ui->setupUi(this);
    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

//    GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0.2, -0.4), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
//    GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 1, -10), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
    GetRaytracer()->SetCanvas(500,500);

//    GetRaytracer()->LoadNewMesh("SimpleBezierTriangle2.obj");

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
    QVector3D vPos(ui->PosX->value(), ui->PosY->value(), ui->PosZ->value());
    QVector3D vTarget(ui->DirX->value(), ui->DirY->value(), ui->DirZ->value());
    QVector3D vUp(ui->UpX->value(), ui->UpY->value(), ui->UpZ->value());

    GetRaytracer()->GetCamera().SetCameraPos(vPos, vTarget, vUp);
//    GetRaytracer()->SetCanvas(500,500);

    GetRaytracer()->GetCamera().BeginFrame();

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

void MainWindow::on_RenderBezierCheckBox_toggled(bool checked)
{
    GetSettings()->SetIntersectBezier(checked);
}

void MainWindow::on_NormalSmoothingCheckBox_toggled(bool checked)
{

}
