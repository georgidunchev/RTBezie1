#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>

#include <ray.h>

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = 0);
    
    void SetCameraPos(QVector3D vPos, QVector3D vTarget, QVector3D vUp);
    CRay GetScreenRay(int x, int y);
    void SetAspectRatio(qreal fAspectRatio);
    void SetCameraResolution(qreal fWidth, qreal fHeight);
    //calculate camera properties based on previous set up
    void BeginFrame();

signals:
    
public slots:
    
private:
    QVector3D m_vPos;
    QVector3D m_vTarget;
    QVector3D m_vUp;
    qreal m_fAspectRatio;
    qreal m_fWidth;
    qreal m_fHeight;
    qreal m_fFov; // in degrees

    QVector3D vUpLeftCorner;
    QVector3D vDownRightCorner;
    QVector3D vHorizontal;
    QVector3D vVertical;
};

#endif // CAMERA_H
