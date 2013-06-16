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
    m_vPos = m_vTempPos = vPos;
    m_vTarget = m_vTempTarget = vTarget;
    m_vTarget.normalize();
    m_vUp = vUp;
    m_vUp.normalize();

	m_vTarget = m_vTempTarget;
	m_vPos = m_vTempPos;
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
	m_vTarget = m_vTempTarget;
	m_vPos = m_vTempPos;

    double m_fFovMultiplierX, m_fFovMultiplierY;
    m_fFovMultiplierX = m_fAspectRatio;
    m_fFovMultiplierY = 1;

    double fCurDiagonal = qSqrt(m_fFovMultiplierX*m_fFovMultiplierX + m_fFovMultiplierY*m_fFovMultiplierY);
    double fTargetDiagonal = qTan( (m_fFov / 2.f) * k_fDegreesToRadians) * 2;
    double fFactor = fTargetDiagonal / fCurDiagonal;
    m_fFovMultiplierX *= fFactor;
    m_fFovMultiplierY *= fFactor;

//    QVector3D vCameraCentre = m_vPos + m_vTarget;
    QVector3D vDir = m_vTarget - m_vPos;

    //find new up dir

    QVector3D vRight = QVector3D::normal(vDir, m_vUp);
    QVector3D vTrueUp = QVector3D::normal(vDir, vRight);

    m_vUpLeftCorner = m_vTarget + m_fFovMultiplierY * vTrueUp / 2 - m_fFovMultiplierX * vRight / 2;
    QVector3D vUpRightCorner = m_vTarget + m_fFovMultiplierY * vTrueUp / 2 + m_fFovMultiplierX * vRight / 2;
    QVector3D vDownLeftCorner = m_vTarget - m_fFovMultiplierY * vTrueUp / 2 - m_fFovMultiplierX * vRight / 2;
    vHorizontal = vUpRightCorner - m_vUpLeftCorner;
    vVertical = vDownLeftCorner - m_vUpLeftCorner;
}

CRay Camera::GetScreenRay(int x, int y)
{
    QVector3D vDest = m_vUpLeftCorner + vHorizontal * (x / m_fWidth)
			 + vVertical * (y / m_fHeight);
//    qDebug()<<vDest - m_vPos;
    CRay rResult(m_vPos, vDest - m_vPos);
//    CRay rResult(m_vPos, m_vDir);
    return rResult;
}

void Camera::Rotate(float fX, float fY)
{
    //Get Vector from LookAt to camera, thats what i want to rotate.
	QVector3D vCamera = -m_vTempTarget + m_vTempPos;
    float fDistance = vCamera.length();

    //do rotation
    QQuaternion qLocalX = QQuaternion::fromAxisAndAngle(0.0,1.0,0.0,fX/100.0f);
    QQuaternion qLocalY = QQuaternion::fromAxisAndAngle(1.0,0.0,0.0,fY/100.0f);

    QQuaternion qLocal = qLocalX * qLocalY;

    vCamera = qLocal.rotatedVector(vCamera);

    vCamera = vCamera.normalized() * fDistance;

    //Set new position
    m_vTempPos = m_vTempTarget + vCamera;
}

void Camera::MoveTarget(float fX, float fY)
{
	QVector3D vCamera = -m_vTempTarget + m_vTempPos;
	QVector3D vRight =  QVector3D::crossProduct(vCamera, m_vUp);
	vRight.normalize();

	m_vTempTarget += -vRight * fX * 0.001f + m_vUp * fY * 0.001f;
}

void Camera::Zoom(float fZoom)
{
   	QVector3D vCamera = -m_vTempTarget + m_vTempPos;
    float fDistance = vCamera.length();

	if (fZoom < 0)
	{
		fDistance *= 1.1f;
	}
	else
	{
		fDistance *= 0.9f;
	}

    vCamera = vCamera.normalized() * fDistance;

    m_vTempPos = m_vTempTarget + vCamera;
}