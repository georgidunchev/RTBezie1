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
	, m_bAutoRendering(false)
	, m_bStartNormalRender(false)
	, m_bShouldRefreshView(false)
{
	ui->setupUi(this);
	GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0.2, -0.4), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
	GetRaytracer()->SetCanvas(250,250);

	//    GetRaytracer()->LoadNewMesh("SimpleBezierTriangle2.obj");

	progress.setLabelText("Rendering");
	progress.setWindowModality(Qt::WindowModal);
}

void MainWindow::paintEvent(QPaintEvent *pe)
{
	//    ui->graphicsView->scene()->clear();

	ui->Image->setDisabled(false);

	if (m_bShouldRefreshView)
	{
		QMainWindow::paintEvent(pe);

		if (m_bAutoRendering)
		{
			StartRender(false);
		}
		else if (m_bStartNormalRender)
		{
			m_bStartNormalRender = false;
			StartRender();
		}
	}
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
	if (m_bAutoRendering)
	{
		m_bStartNormalRender = true;
	}

	m_bAutoRendering = false;
	ui->AutoRender->setEnabled(false);
	ui->AutoRender->setChecked(false);

	progress.setMaximum( GetRaytracer()->GetBucketsCount() );

	QObject::connect(GetRaytracer(), SIGNAL(sigBucketDone(int)), &progress, SLOT(setValue(int)));
	QObject::connect(GetRaytracer(), SIGNAL(sigThreadsFinished()), this, SLOT(slotRenderFinished()));

	StartRender();
}

void MainWindow::on_AutoRender_clicked(bool checked)
{
	m_bAutoRendering = checked;

	if (m_bAutoRendering)
	{
		progress.setMaximum( GetRaytracer()->GetBucketsCount() );

		QObject::connect(GetRaytracer(), SIGNAL(sigBucketDone(int)), &progress, SLOT(setValue(int)));
		QObject::connect(GetRaytracer(), SIGNAL(sigThreadsFinished()), this, SLOT(slotRenderFinished()));

		StartRender(false);
	}
}


void MainWindow::slotRenderFinished()
{
	progress.reset();

	ui->Image->setPixmap(QPixmap::fromImage(GetRaytracer()->GetImage()));

	if (GetRaytracer()->GetTimer().isValid())
	{
		qDebug() << "Render Finished in" << GetRaytracer()->GetTimer().elapsed() / 1000.f;
		GetRaytracer()->GetTimer().invalidate();
	}

	ui->AutoRender->setEnabled(true);
	m_bShouldRefreshView = true;
}

void MainWindow::on_RenderBezierCheckBox_toggled(bool checked)
{
	GetSettings()->SetIntersectBezier(checked);
}

void MainWindow::on_NormalSmoothingCheckBox_toggled(bool checked)
{

}

void MainWindow::StartRender(bool bHighQuality)
{
	//    QVector3D vPos(ui->PosX->value(), ui->PosY->value(), ui->PosZ->value());
	//    QVector3D vTarget(ui->DirX->value(), ui->DirY->value(), ui->DirZ->value());
	//    QVector3D vUp(ui->UpX->value(), ui->UpY->value(), ui->UpZ->value());

	//    GetRaytracer()->GetCamera().SetCameraPos(vPos, vTarget, vUp);
	//    GetRaytracer()->SetCanvas(500,500);

	GetRaytracer()->GetCamera().BeginFrame();
	GetRaytracer()->GetTimer().start();
	GetRaytracer()->RenderThreaded(bHighQuality);
	//GetRaytracer()->Render();
}
