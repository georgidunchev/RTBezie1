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
	, m_bRendering(false)
{
	ui->setupUi(this);
	GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0, -0.4), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
	//GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0.2, -0.4), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
	GetRaytracer()->SetCanvas(500,500);
	
	//    LoadNewMesh("Triangle.obj");
	//    LoadNewMesh("bunny_200.obj");
	//    LoadNewMesh("SimpleBezierTriangle1.obj");
	GetRaytracer()->LoadNewMesh("triangle2.obj");

	//    GetRaytracer()->LoadNewMesh("SimpleBezierTriangle2.obj");

	//QObject::connect(this, SIGNAL(DebugOutChanged(const QString &)), ui->Output, SLOT(setText(QString)));

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
		emit DebugOutChanged(GetUtils()->strDebugOut);

		if (m_bStartNormalRender)
		{
			m_bStartNormalRender = false;
			m_bAutoRendering = false;
			m_bAutoRendering = false;
			ui->AutoRender->setEnabled(false);
			ui->AutoRender->setChecked(false);
			StartRender();
		}
		else if (m_bAutoRendering)
		{
			StartRender(false);
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
		return;
	}

	ui->AutoRender->setEnabled(false);

	//progress.setMaximum( GetRaytracer()->GetBucketsCount() );

	QObject::connect(GetRaytracer(), SIGNAL(sigBucketDone(int)), &progress, SLOT(setValue(int)), Qt::UniqueConnection);
	QObject::connect(GetRaytracer(), SIGNAL(sigThreadsFinished()), this, SLOT(slotRenderFinished()), Qt::UniqueConnection);

	if (!m_bAutoRendering)
	{
		StartRender();
	}
}

void MainWindow::on_AutoRender_clicked(bool checked)
{
	m_bAutoRendering = checked;

	//ui->StartRender->setEnabled(m_bAutoRendering);
	//ui->StartRender->setChecked(m_bAutoRendering);

	if (m_bAutoRendering)
	{
		//progress.setMaximum( GetRaytracer()->GetBucketsCount() );

		QObject::disconnect(GetRaytracer(), SIGNAL(sigBucketDone(int)), &progress, SLOT(setValue(int)));
		QObject::connect(GetRaytracer(), SIGNAL(sigThreadsFinished()), this, SLOT(slotRenderFinished()), Qt::UniqueConnection);
		if (!m_bRendering)
		{
			StartRender(false);
		}
	}
}


void MainWindow::slotRenderFinished()
{
	//progress.reset();

	ui->Image->setPixmap(QPixmap::fromImage(GetRaytracer()->GetImage()));

	if (GetRaytracer()->GetTimer().isValid())
	{
		qDebug() << "Render Finished in" << GetRaytracer()->GetTimer().elapsed() / 1000.f;
		GetRaytracer()->GetTimer().invalidate();
	}

	ui->AutoRender->setEnabled(true);
	m_bShouldRefreshView = true;

	m_bRendering = false;
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
	if(m_bRendering)
	{
		return;
	}
	m_bRendering = true;
	GetRaytracer()->GetTimer().start();
	GetRaytracer()->BeginFrame(bHighQuality);
	GetRaytracer()->RenderThreaded();
	//GetRaytracer()->Render();
}

void MainWindow::DisplayText(const std::string& strOutput)
{
    m_strOutput += strOutput;
    ui->Output->setText(m_strOutput.c_str());
}
