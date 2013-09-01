#ifndef INTERSACTIONINFO_H
#define INTERSACTIONINFO_H

#include <QVector3D>
#include <color.h>

class CSubTriangle;

class CIntersactionInfo
{
public:
    CIntersactionInfo()
		: m_fDistance(10e38)
		, color(0.5, 0.5, 0.5)
		, m_bInitialRay(true)
	{};
    qreal m_fDistance;
    QVector3D m_vIntersectionPoint;
    QVector3D m_vNormal;
	float u, v, w;
	CColor color;
	int m_nTriangleId;
	int m_nSubTriangleId;
	CSubTriangle * pSubTriangle;
	bool m_bInitialRay;
};

#endif // INTERSACTIONINFO_H
