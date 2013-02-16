#ifndef INTERSACTIONINFO_H
#define INTERSACTIONINFO_H

#include <QVector3D>

class CIntersactionInfo
{
public:
    CIntersactionInfo();

    QVector3D GetIntersectionPoint();
    qreal m_fDistance;
    QVector3D m_vNormal;

};

#endif // INTERSACTIONINFO_H
