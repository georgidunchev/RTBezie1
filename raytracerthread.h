#ifndef RAYTRACERTHREAD_H
#define RAYTRACERTHREAD_H

#include <QThread>
class CRaytracerThread: public QThread
{
public:
    CRaytracerThread(int nThreadID);
    void run();
private:
    int m_nBucketId;
    int m_nThreadID;
};

#endif // RAYTRACERTHREAD_H
