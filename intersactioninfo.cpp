#include "intersactioninfo.h"
#include "Utils.h"
#include "color.h"

CVector3DF CIntersactionInfo::GetFacingNormal(const CRay &ray) const
{
    const float fDot = m_vNormal.Dot(ray.Direction());
    const float fMod = (fDot > 0.0f) ? -1.f : 1.f;

    return fMod * m_vNormal;
}
