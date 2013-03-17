#include "camera.h"
#include "qmath.h"
#include <QDebug>

Camera::Camera(QObject *parent) :
    QObject(parent)
{
    m_fFov = 90.f;
}

void Camera::SetCameraPos(QVector3D vPos, QVector3D vTarget, QVector3D vUp)
{
    m_vPos = vPos;
    m_vTarget = vTarget;
    m_vTarget.normalize();
    m_vUp = vUp;
    m_vUp.normalize();


}
void Camera::SetAspectRatio(qreal fAspectRatio)
{
    m_fAspectRatio = fAspectRatio;
}

void Camera::SetCameraResolution(qreal fWidth, qreal fHeight)
{
    m_fWidth = fWidth;
    m_fHeight = fHeight;
    SetAspectRatio(fWidth / fHeight);
}

void Camera::BeginFrame()
{
    qreal fFovMultiplierX, fFovMultiplierY;
    fFovMultiplierX = m_fAspectRatio;
    fFovMultiplierY = 1;

    const qreal Pi = 3.14159f;
    const qreal fDegreesToRadians = Pi/180.f;
    qreal fCurDiagonal = qSqrt(fFovMultiplierX*fFovMultiplierX + fFovMultiplierY*fFovMultiplierY);
    qreal fTargetDiagonal = qTan( (m_fFov / 2.f) * fDegreesToRadians ) * 2;
    qreal fFactor = fTargetDiagonal / fCurDiagonal;
    fFovMultiplierX *= fFactor;
    fFovMultiplierY *= fFactor;

//    QVector3D vCameraCentre = m_vPos + m_vTarget;
    QVector3D vDir = m_vTarget - m_vPos;

    //find new up dir

    QVector3D vRight = QVector3D::normal(vDir, m_vUp);
    QVector3D vTrueUp = QVector3D::normal(vDir, vRight);

    vUpLeftCorner = m_vTarget + fFovMultiplierY * vTrueUp / 2 - fFovMultiplierX * vRight / 2;
    QVector3D vUpRightCorner = m_vTarget + fFovMultiplierY * vTrueUp / 2 + fFovMultiplierX * vRight / 2;
    QVector3D vDownLeftCorner = m_vTarget - fFovMultiplierY * vTrueUp / 2 - fFovMultiplierX * vRight / 2;
    vHorizontal = vUpRightCorner - vUpLeftCorner;
    vVertical = vDownLeftCorner - vUpLeftCorner;
}

CRay Camera::GetScreenRay(int x, int y)
{
    QVector3D vDest = vUpLeftCorner + vHorizontal * (x / m_fWidth)
			 + vVertical * (y / m_fHeight);
//    qDebug()<<vDest - m_vPos;
    CRay rResult(m_vPos, vDest - m_vPos);
//    CRay rResult(m_vPos, m_vDir);
    return rResult;
}
