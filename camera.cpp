#include "camera.h"
#include "qmath.h"
#include <QDebug>
#include "Utils.h"
#include <QQuaternion>
#include "main.h"
#include "raytracer.h"
#include "cmesh.h"

Camera::Camera(QObject *parent) :
    QObject(parent)
{
    m_fFov = 90.f;
}

void Camera::SetCameraPos(QVector3D vPos, QVector3D vTarget, QVector3D vUp)
{
    m_vTempPos = vPos;
    m_vTempTarget = vTarget;
//    m_vTempTarget.normalize();
    m_vUp = vUp;
    m_vUp.normalize();

    m_vTarget = m_vTempTarget;
    m_vPos = m_vTempPos;
}

CVector3DF Camera::GetCameraPos() const
{
    return m_vPos;
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
    QVector3D vDir = (m_vTarget - m_vPos).normalized();

    //find new up dir

    QVector3D vRight = QVector3D::normal(vDir, m_vUp).normalized();
    QVector3D vTrueUp = QVector3D::normal(vDir, vRight).normalized();

    m_vUpLeftCorner = m_vPos + vDir + m_fFovMultiplierY * vTrueUp * 0.5f - m_fFovMultiplierX * vRight * 0.5f;
    QVector3D vUpRightCorner = m_vPos + vDir + m_fFovMultiplierY * vTrueUp * 0.5f + m_fFovMultiplierX * vRight * 0.5f;
    QVector3D vDownLeftCorner = m_vPos + vDir - m_fFovMultiplierY * vTrueUp * 0.5f - m_fFovMultiplierX * vRight * 0.5f;
    vHorizontal = vUpRightCorner - m_vUpLeftCorner;
    vVertical = vDownLeftCorner - m_vUpLeftCorner;
}

CRay Camera::GetScreenRay(int x, int y)
{
    QVector3D vDest = m_vUpLeftCorner + vHorizontal * (x / m_fWidth)
			 + vVertical * (y / m_fHeight);
    CRay rResult(m_vPos, vDest - m_vPos);
    return rResult;
}

void Camera::Rotate(float fX, float fY)
{
    //Get Vector from LookAt to camera, thats what i want to rotate.
	QVector3D vCamera = -m_vTempTarget + m_vTempPos;
//    vCamera.normalize();
	QVector3D vRight =  QVector3D::crossProduct(vCamera, m_vUp);
	vRight.normalize();
    float fDistance = vCamera.length();

    //do rotation
    QQuaternion qLocalX = QQuaternion::fromAxisAndAngle(0.0, 1.0, 0.0, fX / 10.0f);
    QQuaternion qLocalY = QQuaternion::fromAxisAndAngle(vRight, -fY / 10.0f);

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

    if (fZoom < 0.0f)
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

void Camera::Reset()
{
    const QVector3D vTarget = GetRaytracer()->GetMesh().GetAABB().GetCentre().GetQVector();
    SetCameraPos(QVector3D(0, vTarget.y(), -0.4f), vTarget, QVector3D(0, -1, 0) );
}

void Camera::LookAtCentre()
{
    const QVector3D vTarget = GetRaytracer()->GetMesh().GetAABB().GetCentre().GetQVector();
    m_vTempTarget = QVector3D(0, 0, 0);
}
