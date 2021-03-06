#include <QDebug>
#include <QFileDialog>
#include <QProgressDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "raytracer.h"
#include "main.h"
#include "cmesh.h"
#include "settings.h"
#include "kdtreenode.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_bAutoRendering(false)
    , m_bStartNormalRender(false)
    , m_bShouldRefreshView(false)
    , m_bRendering(false)
{
    ui->setupUi(this);

    progress.setWindowModality(Qt::WindowModal);

    QObject::connect(&GetRaytracer()->GetMesh(), SIGNAL(sigLoadingStarted(int)), this, SLOT(slotLoadingStarted(int)), Qt::UniqueConnection);
    QObject::connect(&GetRaytracer()->GetMesh(), SIGNAL(sigLoadingStepDone(int)), &progress, SLOT(setValue(int)), Qt::UniqueConnection);
    QObject::connect(&GetRaytracer()->GetMesh(), SIGNAL(sigLoadingFinished()), this, SLOT(slotLoadingFinished()), Qt::UniqueConnection);

    GetRaytracer()->GetCamera().Reset();//SetCameraPos(QVector3D(0, 0.1, -0.4), QVector3D(0, 0, 0), QVector3D(0, -1, 0) );
    //GetRaytracer()->GetCamera().SetCameraPos(QVector3D(0, 0.2, -0.4), QVector3D(0, 0, 1), QVector3D(0, -1, 0) );
    GetRaytracer()->SetCanvas(500,500);

    GetSettings()->SetNofSubdivisions(static_cast<uint>(ui->NumberOfSubdivisions->value()));
    GetSettings()->m_nThreads = ui->NofThreadsSpinBox->value();
    GetSettings()->m_nIterations = ui->NofIterationsSpinBox->value();

    GetSettings()->m_bUseKDTtee = ui->UseKDTreeCheckbox->isChecked();
    GetSettings()->m_bShowKDTtee = ui->ShowKDTreeCheckbox->isChecked();;
    GetSettings()->m_bNormalSmoothing = ui->NormalSmoothingCheckBox->isChecked();
    GetSettings()->m_bMultipleSeeds = ui->MultiSeedCheckBox->isChecked();
    GetSettings()->m_bWireframe = ui->WireframeCheckBox->isChecked();
    GetSettings()->m_bHalfSubdvision = ui->HalfRB->isChecked();

    //    LoadNewMesh("Triangle.obj");
    //    LoadNewMesh("bunny_200.obj");
    //    LoadNewMesh("SimpleBezierTriangle1.obj");
    //GetRaytracer()->LoadNewMesh("triangle2.obj");

//    GetRaytracer()->LoadNewMesh("SimpleBezierTriangle1.obj");
}

void MainWindow::paintEvent(QPaintEvent *pe)
{
    //    ui->graphicsView->scene()->clear();

    ui->Image->setDisabled(false);

    if (m_bShouldRefreshView)
    {
        QMainWindow::paintEvent(pe);

        if (m_bStartNormalRender)
        {
            m_bAutoRendering = false;
            StartSingleRender();
        }
        else if (m_bAutoRendering)
        {
            StartRender(false);
        }
        else
        {
            EnableRefreshAndLoad(true);
        }
        m_bShouldRefreshView = false;
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

    if (strFileName.isEmpty())
    {
        return;
    }

    GetSettings()->SetNofSubdivisions(static_cast<uint>(ui->NumberOfSubdivisions->value()));
    GetSettings()->m_bHalfSubdvision = ui->HalfRB->isChecked();

    GetRaytracer()->LoadNewMesh(strFileName);

    strFileName.remove(0,strFileName.lastIndexOf("/")+1);
    ui->labelFileName->setText(strFileName);

    emit EnableRenderButton(true);
}

void MainWindow::on_StartRender_clicked()
{ 
    if (!GetRaytracer()->GetMesh().isMeshLoaded()) return;

    if (m_bAutoRendering)
    {
        m_bStartNormalRender = true;
        return;
    }

    StartSingleRender();
}

void MainWindow::on_AutoRender_clicked(bool checked)
{
    if (!GetRaytracer()->GetMesh().isMeshLoaded())
    {
        ui->AutoRender->setChecked(false);
        return;
    }

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
    progress.reset();
    progress.setVisible(false);

    ui->Image->setPixmap(QPixmap::fromImage(GetRaytracer()->GetImage()));

    if (GetRaytracer()->GetTimer().isValid())
    {
        QString str;
        QTextStream stream;
        stream.setString(&str);
        int a = GetRaytracer()->m_nAABBIntersections.fetchAndStoreRelaxed(0);
        int b = GetRaytracer()->m_nObjIntersections.fetchAndStoreRelaxed(0);
        int c = GetRaytracer()->m_nAABBTime.fetchAndStoreRelaxed(0);
        int d = GetRaytracer()->m_nObjTime.fetchAndStoreRelaxed(0);
        int e = GetRaytracer()->m_nTotalTime.fetchAndStoreRelaxed(0);
        int f = GetRaytracer()->GetMesh().GetMemorySubKB();
        int g = GetRaytracer()->GetMesh().GetMemoryKDtreeKB();
        stream << "Tests: AABB " << a << " Obj " << b
               << "\nTime: AABB "<< c << " + Obj "<< d << " = " << c+d
               << "\nSubTriangles: " << GetRaytracer()->GetMesh().m_nNOfSubtriangles << " AABBs: " << GetRaytracer()->GetMesh().n_mLeafs
               << "\n" << ((f+g>1024)?"MB: ":"KB: ") << ((f+g>1024)?f/1024:f) << "/" << ((f+g>1024)?g/1024:g)
               << "\nRender Finished in: " << GetRaytracer()->GetTimer().elapsed() << " / " << e;
        GetRaytracer()->GetTimer().invalidate();
        DisplayText(str);
    }

    ui->AutoRender->setEnabled(true);
    m_bShouldRefreshView = true;

    m_bRendering = false;
}

void MainWindow::slotLoadingStarted(int nMaxSteps)
{
    progress.reset();
    progress.setLabelText("Loading");
    progress.setMaximum(nMaxSteps);
    progress.show();
}

void MainWindow::slotLoadingFinished()
{
    progress.reset();
    progress.setVisible(false);
}

//void MainWindow::on_RenderBezierCheckBox_toggled(bool checked)
//{
//    GetSettings()->SetIntersectBezier(checked);
//}

//void MainWindow::on_NormalSmoothingCheckBox_toggled(bool checked)
//{

//}

void MainWindow::StartSingleRender()
{
    m_bStartNormalRender = false;

    ui->AutoRender->setEnabled(false);
    ui->AutoRender->setChecked(false);

    progress.setLabelText("Rendering");
    progress.setMaximum( GetRaytracer()->GetBucketsCount() );
    progress.show();

    QObject::connect(GetRaytracer(), SIGNAL(sigBucketDone(int)), &progress, SLOT(setValue(int)), Qt::UniqueConnection);
    QObject::connect(GetRaytracer(), SIGNAL(sigThreadsFinished()), this, SLOT(slotRenderFinished()), Qt::UniqueConnection);

//    QObject::connect(GetRaytracer()->GetMesh(), SIGNAL(sigLoadedMesh(bool)), this, SLOT(ui->AutoRender->setEnabled(bool)), Qt::UniqueConnection);


    if (!m_bAutoRendering)
    {
        StartRender();
    }

//    EnableRefreshAndLoad(false);
}

void MainWindow::StartRender(bool bHighQuality)
{
    bHighQuality = ui->BezierRB_3->isChecked();
    if(m_bRendering)
    {
        return;
    }

    GetSettings()->m_nThreads = ui->NofThreadsSpinBox->value();
    GetSettings()->m_nIterations = ui->NofIterationsSpinBox->value();

    m_bRendering = true;
    GetRaytracer()->GetTimer().start();
    GetRaytracer()->BeginFrame(bHighQuality);
    GetRaytracer()->RenderThreaded();
    //GetRaytracer()->Render();

    EnableRefreshAndLoad(false);
}

void MainWindow::DisplayText(const QString& strOutput)
{
    ui->Output->setText(strOutput);
}

void MainWindow::on_NumberOfSubdivisions_valueChanged(int arg1)
{
//    GetSettings()->SetNofSubdivisions(static_cast<uint>(arg1));
}

void MainWindow::on_butRefresh_clicked()
{
    const std::string& strFilename = GetRaytracer()->GetOpenFileName();
    GetSettings()->SetNofSubdivisions(static_cast<uint>(ui->NumberOfSubdivisions->value()));
    GetSettings()->m_bHalfSubdvision = ui->HalfRB->isChecked();

    if (strFilename.empty())
    {
        return;
    }

    GetRaytracer()->LoadNewMesh(strFilename.c_str());
}

void MainWindow::on_NormalSmoothingCheckBox_clicked(bool checked)
{
    GetSettings()->m_bNormalSmoothing = checked;
}

void MainWindow::on_MultiSeedCheckBox_clicked(bool checked)
{
    GetSettings()->m_bMultipleSeeds = checked;
}

void MainWindow::on_UseKDTreeCheckbox_clicked(bool checked)
{
    GetSettings()->m_bUseKDTtee = checked;
}

void MainWindow::on_ShowKDTreeCheckbox_clicked(bool checked)
{
    GetSettings()->m_bShowKDTtee = checked;
}

void MainWindow::on_WireframeCheckBox_clicked(bool checked)
{
    GetSettings()->m_bWireframe = checked;
}

void MainWindow::on_butResetCamera_clicked()
{
    GetRaytracer()->GetCamera().Reset();
}

void MainWindow::on_butLookAtCentre_clicked()
{
    GetRaytracer()->GetCamera().LookAtCentre();
}

void MainWindow::EnableRefreshAndLoad(bool bEnable)
{
    ui->butRefresh->setEnabled(bEnable);
    ui->openMeshButton->setEnabled(bEnable);
}
