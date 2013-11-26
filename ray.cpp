#include "ray.h"

CRay::CRay(CVector3DF v3dOrigin, CVector3DF v3dDirection)
{
    m_v3dOrigin = v3dOrigin;
    m_v3dDirection = v3dDirection;
    m_v3dDirection.Normalize();
}

CVector3DF CRay::StartPoint() const
{
    return m_v3dOrigin;
}

CVector3DF CRay::Direction() const
{
    return m_v3dDirection;
}

CVector3DF CRay::GetPointAtDistance(float fDistance) const
{
    CVector3DF vNewPos(m_v3dDirection);
    vNewPos *= fDistance;
    vNewPos += m_v3dOrigin;
    return vNewPos;
}
