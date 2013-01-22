#include "raytracer.h"
#include <cmesh.h>
#include <triangle.h>
#include <ray.h>
#include <camera.h>
#include <raytracerthread.h>
#include <QDebug>

RayTracer::RayTracer(QObject *parent)
    : QObject(parent)
    , m_Camera(this)
    , m_pMesh(NULL)
    , m_nRunningThreads(0)
{
    m_nThreads = 4;
    m_nHorizontalBuckets = 10;
    m_nVerticalBuckets = 10;
}


RayTracer::~RayTracer()
{
    delete m_pImage;
}

void RayTracer::SetCanvas(qreal fWidth, qreal fHeight)
{
    m_nWidth = fWidth;
    m_nHeight = fHeight;

//    m_pMesh = new CMesh();
////    m_Mesh->Load("Triangle.obj");
//    m_pMesh->Load("bunny_1k.obj");

    GetCamera().SetCameraResolution(m_nWidth, m_nHeight);
    GetCamera().BeginFrame();

    m_pImage = new QImage(m_nWidth, m_nHeight, QImage::Format_ARGB32);
}

void RayTracer::Render()
{
    for( int j = 0; j < m_nHeight; ++j)
	for (int i = 0; i < m_nWidth; ++i )
	{
//	    if (i == 1 && j == 5)
//	    {
//		bool b = true;
//		b = false;
//	    }
	    QRgb value = qRgb(0,0,0);
	    if ( m_pMesh )
	    {
		CIntersactionInfo intersectionInfo;
//		GetCamera().getCameraToViewportRay(.getCameraToViewportRay(i, j);
		if ( m_pMesh->Intersect(GetCamera().GetScreenRay(i, j), intersectionInfo) )
		{
		    value = qRgb(189, 149, 39);
		}
	    }

	    m_pImage->setPixel(i, j, value);
	}
}

void RayTracer::RenderThreaded()
{
    for( int j = 0; j < m_nThreads; ++j)
    {
	CRaytracerThread* pThread = new CRaytracerThread(j);
	m_arrThreads.push_back(pThread);

	pThread->start();

	QObject::connect(pThread, SIGNAL(ThreadStarted(int)), this, SLOT(ThreadStarted(int)));
	QObject::connect(pThread, SIGNAL(ThreadEnded(int)), this, SLOT(ThreadEnded(int)));
    }

//    for( int j = 0; j < m_arrThreads.size(); ++j)
//    {
//	m_arrThreads[j]->wait();
//    }
}

QImage& RayTracer::GetImage()
{
    return *m_pImage;
}

CMesh& RayTracer::GetMesh()
{
    if (!m_pMesh)
    {
	m_pMesh = new CMesh(this);
    }
    return *m_pMesh;
}

Camera &RayTracer::GetCamera()
{
    return m_Camera;
}

int RayTracer::GetBucketsCount() const
{
    return m_nVerticalBuckets * m_nHorizontalBuckets;
}

int RayTracer::GetNextBucketId()
{
    if ( m_nNextBucket >= GetBucketsCount())
    {
	emit sigBucketDone(GetBucketsCount());
	return -1;
    }

    emit sigBucketDone(m_nNextBucket);

    return m_nNextBucket.fetchAndAddRelaxed(1);
}

void RayTracer::GetBucketRectById(int nBucketId, QRect &rect) const
{
    int nCol = nBucketId % m_nHorizontalBuckets;
    int nRow = nBucketId / m_nVerticalBuckets;

    int nBucketWidth = m_nWidth / m_nHorizontalBuckets;
    int nBucketHeight = m_nHeight / m_nVerticalBuckets;

    rect.setLeft(nBucketWidth * nCol);
    rect.setTop(nBucketHeight * nRow);

    if (nCol == m_nHorizontalBuckets-1)
    {
	//last bucket of the row
	nBucketWidth += m_nWidth - m_nHorizontalBuckets * nBucketWidth;
    }
    rect.setWidth(nBucketWidth);

    if (nRow == m_nVerticalBuckets-1)
    {
	//last bucket of the row
	nBucketHeight += m_nHeight - m_nVerticalBuckets * nBucketHeight;
    }
    rect.setHeight(nBucketHeight);
}

void RayTracer::ThreadsFinished()
{
    sigThreadsFinished();
}

void RayTracer::ThreadStarted(int nId)
{
    m_nRunningThreads++;
}

void RayTracer::ThreadEnded(int nId)
{
    m_nRunningThreads--;
    if(m_nRunningThreads <= 0)
    {
	ThreadsFinished();
    }
}
