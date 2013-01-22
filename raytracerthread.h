#ifndef RAYTRACERTHREAD_H
#define RAYTRACERTHREAD_H

#include <QThread>
class CRaytracerThread: public QThread
{
    Q_OBJECT
public:
    CRaytracerThread(int nThreadID);
    virtual ~CRaytracerThread();
    void run();
private:
    int m_nBucketId;
    int m_nThreadID;
signals:
    void ThreadStarted(int nId);
    void ThreadEnded(int nId);
};

#endif // RAYTRACERTHREAD_H
