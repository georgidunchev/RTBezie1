#ifndef INTERSACTIONINFO_H
#define INTERSACTIONINFO_H

#include <QVector3D>
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
    QVector3D GetFacingNormal(const CRay &ray) const;
    qreal m_fDistance;
    QVector3D m_vIntersectionPoint;
    QVector3D m_vNormal;
    QVector3D m_vBarCoordsLocal;
    QVector3D m_vBarCoordsGlobal;
    CColor color;
    int m_nTriangleId;
    int m_nSubTriangleId;
    CSubTriangle * pSubTriangle;
    bool m_bInitialRay;
    bool m_bHighQuality;
};

#endif // INTERSACTIONINFO_H
