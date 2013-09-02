#include "intersactioninfo.h"
#include <Utils.h>
#include <color.h>

QVector3D CIntersactionInfo::GetFacingNormal(const CRay &ray) const
{
    const float fDot = CUtils::Dot(m_vNormal, ray.Direction());
    const float fMod = (fDot > 0.0f) ? -1.f : 1.f;

    return fMod * m_vNormal;
}
