#ifndef INTERSACTIONINFO_H
#define INTERSACTIONINFO_H

#include "vector3df.h"
#include "color.h"
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
        , m_nAABBIntersections(0)
        , m_nBezierIntersections(0)
        , m_nAABBTime(0)
        , m_nObjTime(0)
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

    int m_nAABBIntersections;
    int m_nBezierIntersections;
    qint64 m_nAABBTime;
    qint64 m_nObjTime;
};

#endif // INTERSACTIONINFO_H
