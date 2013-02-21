#ifndef INTERSACTIONINFO_H
#define INTERSACTIONINFO_H

#include <QVector3D>

class CIntersactionInfo
{
public:
    CIntersactionInfo();    
    qreal m_fDistance;
    QVector3D m_vIntersectionPoint;
    QVector3D m_vNormal;

};

#endif // INTERSACTIONINFO_H
