#include "camera.h"
#include "qmath.h"
#include <QDebug>
#include <Utils.h>
#include <QQuaternion>

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
    double fFovMultiplierX, fFovMultiplierY;
    fFovMultiplierX = m_fAspectRatio;
    fFovMultiplierY = 1;

    double fCurDiagonal = qSqrt(fFovMultiplierX*fFovMultiplierX + fFovMultiplierY*fFovMultiplierY);
    double fTargetDiagonal = qTan( (m_fFov / 2.f) * k_fDegreesToRadians) * 2;
    double fFactor = fTargetDiagonal / fCurDiagonal;
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

void Camera::Rotate(float fX, float fY)
{
    //Get Vector from LookAt to camera, thats what i want to rotate.
    QVector3D vCamera = -m_vTarget + m_vPos;
    float fDistance = vCamera.length();

    //do rotation
    QQuaternion qLocalX = QQuaternion::fromAxisAndAngle(0.0,1.0,0.0,fX/100.0f);
    QQuaternion qLocalY = QQuaternion::fromAxisAndAngle(1.0,0.0,0.0,fY/100.0f);

    QQuaternion qLocal = qLocalX * qLocalY;

    vCamera = qLocal.rotatedVector(vCamera);

    vCamera = vCamera.normalized() * fDistance;

    //Set new position
    m_vPos = m_vTarget + vCamera;
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
