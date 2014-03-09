#include "raytracerthread.h"
#include "raytracer.h"
#include "main.h"
#include <QMutex>
#include <QDebug>
#include "intersactioninfo.h"
#include "cmesh.h"
#include "shader.h"

CRaytracerThread::CRaytracerThread(int nThreadID, int nThreads)
    : m_nThreadID(nThreadID)
    , m_nThreads(nThreads)
{

}

CRaytracerThread::~CRaytracerThread()
{

}

void CRaytracerThread::run()
{     
    emit sigThreadStarted(m_nThreadID);
    int nAABBIntersections = 0;
    int nObjIntersections = 0;
    qint64 nAABBTime = 0;
    qint64 nObjeTime = 0;
    qint64 nTotalTime = 0;
    qint64 nShadeTime = 0;
    qint64 nMillion = 1000000;

    QElapsedTimer timer;
    timer.start();

    forever
    {
        m_nBucketId = GetRaytracer()->GetNextBucketId();

        if (m_nBucketId < 0)
        {
            GetRaytracer()->m_nAABBIntersections.fetchAndAddRelaxed(nAABBIntersections);
            GetRaytracer()->m_nObjIntersections.fetchAndAddRelaxed(nObjIntersections);
            nAABBTime /= nMillion;
            nObjeTime /= nMillion;
            int a = nAABBTime;
            int b = nObjeTime;
            GetRaytracer()->m_nAABBTime.fetchAndAddRelaxed(a);
            GetRaytracer()->m_nObjTime.fetchAndAddRelaxed(b);


            nTotalTime = (timer.nsecsElapsed() - nShadeTime) / nMillion;
            int c = nTotalTime;
            GetRaytracer()->m_nTotalTime.fetchAndAddRelaxed(c);
            break;
        }

        QRect bucketRect;
        GetRaytracer()->GetBucketRectById(m_nBucketId, bucketRect);

        for( int j = bucketRect.top(); j <= bucketRect.bottom(); ++j)
            for (int i = bucketRect.left(); i <= bucketRect.right(); ++i )
            {
                QRgb value = qRgb(255, 255, 255);
                CIntersactionInfo intersectionInfo;

                CRay Ray = GetRaytracer()->GetCamera().GetScreenRay(i, j);
                if ( GetRaytracer()->GetMesh().Intersect(Ray, intersectionInfo) )
                {
                    QElapsedTimer timer2;
                    timer2.start();
                    value = GetRaytracer()->GetShader().Shade(Ray, intersectionInfo);
                    nShadeTime += timer2.nsecsElapsed();
                }
                GetRaytracer()->GetImage().setPixel(i, j, value);

                nAABBIntersections += intersectionInfo.m_nAABBIntersections;
                nObjIntersections += intersectionInfo.m_nBezierIntersections;
                nAABBTime += intersectionInfo.m_nAABBTime;
                nObjeTime += intersectionInfo.m_nObjTime;
            }
    }

    emit sigThreadEnded(m_nThreadID);
}
