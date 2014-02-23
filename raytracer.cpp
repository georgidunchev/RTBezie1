#include "raytracer.h"
#include "cmesh.h"
#include "triangle.h"
#include "ray.h"
#include "camera.h"
#include "raytracerthread.h"
#include "settings.h"
#include <QDebug>
#include <QMutexLocker>
#include "main.h"

RayTracer::RayTracer(QObject *parent)
    : QObject(parent)
    , m_Camera(this)
    , m_Mesh(this)
    , m_nRunningThreads(0)
{
    m_nHorizontalBuckets = 10;
    m_nVerticalBuckets = 10;
}


RayTracer::~RayTracer()
{
    delete m_pImage;
}

void RayTracer::BeginFrame(bool bHighQuality)
{
    m_bHighQuality = bHighQuality;

    if (bHighQuality)
    {
	m_nCrntWidth = m_nWidth;
	m_nCrntHeight = m_nHeight;
    }
    else
    {
	m_nCrntWidth = m_nSmallWidth;
	m_nCrntHeight = m_nSmallHeight;
    }

    GetCamera().SetCameraResolution(m_nCrntWidth, m_nCrntHeight);

    QImage* pNewImage = new QImage(m_pImage->scaledToHeight(m_nCrntHeight));
    delete m_pImage;
    m_pImage = pNewImage;

    GetCamera().BeginFrame();
}

void RayTracer::SetCanvas(int fWidth, int fHeight)
{
    m_nCanvasWidth = fWidth;
    m_nCanvasHeight = fHeight;

    m_nWidth = m_nCanvasWidth / 1;
    m_nHeight = m_nCanvasHeight / 1;

    m_nSmallWidth = m_nCanvasWidth / 1;
    m_nSmallHeight = m_nCanvasHeight / 1;

    m_pImage = new QImage(m_nCanvasWidth, m_nCanvasHeight, QImage::Format_ARGB32);
}

void RayTracer::Render()
{
    m_nNextBucket = 0;
    forever
    {
	m_nBucketId = GetNextBucketId();
	if (m_nBucketId < 0)
	{
	    break;
	}

	QRect bucketRect;
	GetBucketRectById(m_nBucketId, bucketRect);

	for( int j = bucketRect.top(); j <= bucketRect.bottom(); ++j)
	    for (int i = bucketRect.left(); i <= bucketRect.right(); ++i )
	    {
		GetImage().setPixel(i, j, RenderPixel(i, j));
	    }
    }
    ThreadsFinished();
}

QRgb RayTracer::RenderPixel(const int x, const int y, bool bDebug)
{
    QRgb value = qRgb(150, 150, 150);
    const float dX = m_nCanvasWidth / m_nCrntWidth;
    const float dY = m_nCanvasHeight / m_nCrntHeight;

    const int nScaledX = dX * x;
    const int nScaledY = dY * y;

    if (nScaledX < 0
	    || nScaledX >= m_nCanvasWidth
	    || nScaledY < 0
	    || nScaledY >= m_nCanvasHeight)
    {
	return value;
    }

    CIntersactionInfo intersectionInfo;

    CRay Ray = GetCamera().GetScreenRay(nScaledX, nScaledY);
    if ( GetMesh().Intersect(Ray, intersectionInfo, bDebug) )
    {
	value = GetShader().Shade(Ray, intersectionInfo);
    }

    return value;
}

QRgb RayTracer::RenderPixelFromScreen(const int x, const int y, bool bDebug)
{
    m_nCrntWidth = m_nCanvasWidth;
    m_nCrntHeight = m_nCanvasHeight;
    GetCamera().SetCameraResolution(m_nCrntWidth, m_nCrntHeight);

    if (bDebug)
    {
	qDebug()<<"Checking Pixel x: " << x << " y: " << y;
    }

    return RenderPixel(x, y, bDebug);
}

void RayTracer::RenderThreaded()
{
    m_nNextBucket = 0;

    for( uint j = 0; j < GetSettings()->m_nThreads; ++j)
    {
	CRaytracerThread* pThread;
	if ( m_arrThreads.size() <= j)
	{
	    pThread = new CRaytracerThread(j);
	    m_arrThreads.push_back(pThread);
	    QObject::connect(pThread, SIGNAL(sigThreadStarted(int)), this, SLOT(slotThreadStarted(int)), Qt::UniqueConnection);
	    QObject::connect(pThread, SIGNAL(sigThreadEnded(int)), this, SLOT(slotThreadEnded(int)), Qt::UniqueConnection);
	}
	else
	{
	    pThread = m_arrThreads[j];
	}

	pThread->start();
    }
}

QImage& RayTracer::GetImage()
{
    return *m_pImage;
}

CMesh& RayTracer::GetMesh()
{
    return m_Mesh;
}
bool RayTracer::IsHighQuality() const
{
    return m_bHighQuality;
}

std::string RayTracer::GetOpenFileName() const
{
    return m_strOpenFile;
}

void RayTracer::LoadNewMesh(const QString& strInputFileName)
{
    m_strOpenFile = strInputFileName.toStdString();
    m_Mesh.Load(strInputFileName);
}

Camera &RayTracer::GetCamera()
{
    return m_Camera;
}

CLightsScene &RayTracer::GetLightScene()
{
    return m_LightScene;
}

CShader &RayTracer::GetShader()
{
    return m_Shader;
}

int RayTracer::GetBucketsCount() const
{
    return m_nVerticalBuckets * m_nHorizontalBuckets;
}

int RayTracer::GetNextBucketId()
{
    //QMutexLocker locker(&mutex);
    int nTemp = m_nNextBucket.fetchAndAddRelaxed(1);

    if ( nTemp >= GetBucketsCount())
    {
	//emit sigBucketDone(GetBucketsCount());
	return -1;
    }

    emit sigBucketDone(nTemp);

    return nTemp;
}

void RayTracer::GetBucketRectById(int nBucketId, QRect &rect) const
{
    int nCol = nBucketId % m_nHorizontalBuckets;
    int nRow = nBucketId / m_nVerticalBuckets;

    int nBucketWidth = m_nCrntWidth / m_nHorizontalBuckets;
    int nBucketHeight = m_nCrntHeight / m_nVerticalBuckets;

    rect.setLeft(nBucketWidth * nCol);
    rect.setTop(nBucketHeight * nRow);

    if (nCol == m_nHorizontalBuckets-1)
    {
	//last bucket of the row
	nBucketWidth += m_nCrntWidth - m_nHorizontalBuckets * nBucketWidth;
    }
    rect.setWidth(nBucketWidth);

    if (nRow == m_nVerticalBuckets-1)
    {
	//last bucket of the row
	nBucketHeight += m_nCrntHeight - m_nVerticalBuckets * nBucketHeight;
    }
    rect.setHeight(nBucketHeight);
}

void RayTracer::ThreadsFinished()
{
    QImage* pNewImage = new QImage(m_pImage->scaledToHeight(m_nCanvasHeight));
    delete m_pImage;
    m_pImage = pNewImage;

    emit sigThreadsFinished();
}


void RayTracer::slotThreadStarted(int nId)
{
    //QMutexLocker locker(&mutex);
    m_nRunningThreads++;
    //qDebug() << "Started Thread: "<< m_nRunningThreads;
}

void RayTracer::slotThreadEnded(int nId)
{
    //QMutexLocker locker(&mutex);
    //qDebug() << "Finished Thread 1: "<< m_nRunningThreads;
    int n_local = --m_nRunningThreads;
    if(n_local <= 0)
    {
	ThreadsFinished();
    }
}

void RayTracer::GenerateKDTree()
{

}
