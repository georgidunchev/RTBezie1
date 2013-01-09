#include "ray.h"

CRay::CRay(QVector3D v3dOrigin, QVector3D v3dDirection)
{
    m_v3dOrigin = v3dOrigin;
    m_v3dDirection = v3dDirection;
    m_v3dDirection.normalize();
}

QVector3D CRay::StartPoint() const
{
    return m_v3dOrigin;
}

QVector3D CRay::Direction() const
{
    return m_v3dDirection;
}
