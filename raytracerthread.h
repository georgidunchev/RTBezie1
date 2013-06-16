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
	void SetHighQuality(bool bHighQuality = true);
private:
    int m_nBucketId;
    int m_nThreadID;
	bool m_bHighQuality;
signals:
    void sigThreadStarted(int nId);
    void sigThreadEnded(int nId);
};

#endif // RAYTRACERTHREAD_H
