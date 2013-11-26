#ifndef INTERSACTIONINFO_H
#define INTERSACTIONINFO_H

#include <vector3df.h>
#include <color.h>
#include "ray.h"

class CSubTriangle;

class CIntersactionInfo
{
public:
    CIntersactionInfo()
		: m_fDistance(10e38)
		, color(0.5, 0.5, 0.5)
		, m_bInitialRay(true)
		, m_bHighQuality(false)
	{}
    CVector3DF GetFacingNormal(const CRay &ray) const;
    qreal m_fDistance;
    CVector3DF m_vIntersectionPoint;
    CVector3DF m_vNormal;
    CVector3DF m_vBarCoordsLocal;
    CVector3DF m_vBarCoordsGlobal;
    CColor color;
    int m_nTriangleId;
    int m_nSubTriangleId;
    CSubTriangle * pSubTriangle;
    bool m_bInitialRay;
    bool m_bHighQuality;
};

#endif // INTERSACTIONINFO_H
