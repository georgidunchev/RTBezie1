#include "raytracerthread.h"
#include <raytracer.h>
#include <main.h>
#include <QMutex>
#include <QDebug>
#include "intersactioninfo.h"
#include "cmesh.h"
#include "shader.h"

CRaytracerThread::CRaytracerThread(int nThreadID)
	: m_nThreadID(nThreadID)
{

}

CRaytracerThread::~CRaytracerThread()
{

}

void CRaytracerThread::run()
{     
	emit sigThreadStarted(m_nThreadID);
	forever
	{
		m_nBucketId = GetRaytracer()->GetNextBucketId();
		if (m_nBucketId < 0)
		{
			break;
		}

		QRect bucketRect;
		GetRaytracer()->GetBucketRectById(m_nBucketId, bucketRect);

		for( int j = bucketRect.top(); j <= bucketRect.bottom(); ++j)
			for (int i = bucketRect.left(); i <= bucketRect.right(); ++i )
			{
				//if (j >= 250 || i >=250)
				//{
				//	qDebug() << "bucket id " << m_nBucketId;
				//}
				QRgb value = qRgb(0,0,0);
				CIntersactionInfo intersectionInfo;

				CRay Ray = GetRaytracer()->GetCamera().GetScreenRay(i, j);
				if ( GetRaytracer()->GetMesh().Intersect(Ray, intersectionInfo) )
				{
					value = GetRaytracer()->GetShader().Shade(Ray, intersectionInfo);
				}
				GetRaytracer()->GetImage().setPixel(i, j, value);
			}

			//	qDebug()<<"Thread"<<m_nThreadID<<"Rendered Bucket"<<m_nBucketId;
	}    
	emit sigThreadEnded(m_nThreadID);
}