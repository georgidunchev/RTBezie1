#ifndef INTERSACTIONINFO_H
#define INTERSACTIONINFO_H

#include <QVector3D>
#include <color.h>

class CIntersactionInfo
{
public:
    CIntersactionInfo();    
    qreal m_fDistance;
    QVector3D m_vIntersectionPoint;
    QVector3D m_vNormal;
	float u, v, w;
	CColor color;
	int m_nTriangleId;
	int m_nSubTriangleId;
};

#endif // INTERSACTIONINFO_H
