#include <raytracer.h>
#include <cmesh.h>
#include <triangle.h>
#include <ray.h>
#include <camera.h>
#include <raytracerthread.h>
#include <settings.h>
#include <QDebug>
#include <QMutexLocker>

RayTracer::RayTracer(QObject *parent)
	: QObject(parent)
	, m_Camera(this)
	, m_pMesh(NULL)
	, m_nRunningThreads(0)   
{
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

	//    LoadNewMesh("Triangle.obj");
	//    LoadNewMesh("bunny_200.obj");
	//    LoadNewMesh("SimpleBezierTriangle1.obj");
	LoadNewMesh("triangle2.obj");

	GetCamera().SetCameraResolution(m_nWidth, m_nHeight);

	m_pImage = new QImage(m_nWidth, m_nHeight, QImage::Format_ARGB32);
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
				QRgb value = qRgb(0,0,0);
				CIntersactionInfo intersectionInfo;

				CRay Ray = GetCamera().GetScreenRay(i, j);
				if ( GetMesh().Intersect(Ray, intersectionInfo) )
				{
					value = GetShader().Shade(Ray, intersectionInfo);
				}
				GetImage().setPixel(i, j, value);
			}
	}
	ThreadsFinished();
}

void RayTracer::RenderThreaded(bool bHighQuality)
{
	m_nNextBucket = 0;

	for( int j = 0; j < k_nTHREADS; ++j)
	{
		CRaytracerThread* pThread;
		if ( m_arrThreads.size() <= j)
		{
			pThread = new CRaytracerThread(j);
			m_arrThreads.push_back(pThread);
		}
		else
		{
			pThread = m_arrThreads[j];
		}
		
		pThread->start();

		QObject::connect(pThread, SIGNAL(sigThreadStarted(int)), this, SLOT(slotThreadStarted(int)));
		QObject::connect(pThread, SIGNAL(sigThreadEnded(int)), this, SLOT(slotThreadEnded(int)));
	}
}

QImage& RayTracer::GetImage()
{
	return *m_pImage;
}

CMesh& RayTracer::GetMesh()
{
	return *m_pMesh;
}

void RayTracer::LoadNewMesh(const QString& strInputFileName)
{
	if (m_pMesh)
	{
		delete m_pMesh;
	}

	m_pMesh = new CMesh(this);
	m_pMesh->Load(strInputFileName);
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
		emit sigBucketDone(GetBucketsCount());
		return -1;
	}

	emit sigBucketDone(nTemp);

	return nTemp;
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
	emit sigThreadsFinished();
}


void RayTracer::slotThreadStarted(int nId)
{
	m_nRunningThreads++;
}

void RayTracer::slotThreadEnded(int nId)
{
	m_nRunningThreads--;
	if(m_nRunningThreads <= 0)
	{
		ThreadsFinished();
	}
}

void RayTracer::GenerateKDTree()
{

}
