#ifndef RAYTRACERTHREAD_H
#define RAYTRACERTHREAD_H

#include <QThread>
class CRaytracerThread: public QThread
{
    Q_OBJECT
public:
    CRaytracerThread(int nThreadID, int nThreads);
    virtual ~CRaytracerThread();
    void run();

private:
    int m_nBucketId;
    int m_nThreadID;
    int m_nThreads;

signals:
    void sigThreadStarted(int nId);
    void sigThreadEnded(int nId);
};

#endif // RAYTRACERTHREAD_H
