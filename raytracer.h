#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <QObject>
#include <QtGlobal>
#include <QImage>
#include <QAtomicInt>

#include <camera.h>

class CRaytracerThread;
class CMesh;

class RayTracer : public QObject
{
    Q_OBJECT
public:
    explicit RayTracer(QObject *parent = 0);
    ~RayTracer();

    void SetCanvas(qreal fWidth, qreal fHeight);
    void Render();
    void RenderThreaded();

    QImage &GetImage();

    CMesh &GetMesh();

    Camera & GetCamera();

    //threaded methods
    int GetNextBucketId();
    void GetBucketRectById(int nBucketId, QRect &rect) const;

private:
    Camera m_Camera;

    QImage * m_pImage;
    int m_nWidth;
    int m_nHeight;

    CMesh* m_pMesh;

    // threaded rendering settings
    int m_nThreads; //number of threads
    QAtomicInt m_nNextBucket;
    int m_nHorizontalBuckets;
    int m_nVerticalBuckets;
    QVector<CRaytracerThread*> m_arrThreads;

signals:

public slots:
};

#endif // RAYTRACER_H
