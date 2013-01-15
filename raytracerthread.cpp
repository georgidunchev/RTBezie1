#include "raytracerthread.h"
#include <raytracer.h>
#include <main.h>
#include <QMutex>
#include <QDebug>
#include "intersactioninfo.h"
#include "cmesh.h"

CRaytracerThread::CRaytracerThread(int nThreadID)
    :m_nThreadID(nThreadID)
{
}

void CRaytracerThread::run()
{
    forever
    {
	m_nBucketId = GetRaytracer()->GetNextBucketId();
	if (m_nBucketId < 0)
	{
	    return;
	}

	QRect bucketRect;
	GetRaytracer()->GetBucketRectById(m_nBucketId, bucketRect);

	for( int j = bucketRect.top(); j < bucketRect.bottom(); ++j)
	    for (int i = bucketRect.left(); i < bucketRect.right(); ++i )
	    {
		QRgb value = qRgb(0,0,0);
		CIntersactionInfo intersectionInfo;

		if ( GetRaytracer()->GetMesh().Intersect(GetRaytracer()->GetCamera().GetScreenRay(i, j), intersectionInfo) )
		{
		    value = qRgb(189, 149, 39);
		}

		//	    QMutexLocker();
		GetRaytracer()->GetImage().setPixel(i, j, value);
	    }

	qDebug()<<"Thread"<<m_nThreadID<<"Rendered Bucket"<<m_nBucketId;
    }
}
