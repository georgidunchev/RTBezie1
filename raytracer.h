#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <QObject>
#include <QtGlobal>
#include <QImage>
#include <QAtomicInt>
#include <QElapsedTimer>

#include <camera.h>
#include <QProgressDialog>

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

    void LoadNewMesh(const QString& strInputFileName);

    Camera & GetCamera();

    QElapsedTimer& GetTimer() { return m_Timer; }
    //threaded methods
    int GetBucketsCount() const;
    int GetNextBucketId();
    void GetBucketRectById(int nBucketId, QRect &rect) const;
    void ThreadsFinished();

    //KD tree
    void GenerateKDTree();

private:
    Camera m_Camera;

    QImage * m_pImage;
    int m_nWidth;
    int m_nHeight;

    CMesh* m_pMesh;

    QElapsedTimer m_Timer;

    // threaded rendering settings
    int m_nThreads; //number of threads
    QAtomicInt m_nNextBucket;
    int m_nHorizontalBuckets;
    int m_nVerticalBuckets;
    QVector<CRaytracerThread*> m_arrThreads;

    QProgressDialog* progress;

    int m_nRunningThreads;

signals:
    void sigBucketDone(int value);
    void sigThreadsFinished();
public slots:
    void slotThreadStarted(int nId);
    void slotThreadEnded(int nId);
};

#endif // RAYTRACER_H
