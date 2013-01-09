#ifndef CRAY_H
#define CRAY_H

#include <QVector3D>

class CRay
{
public:
    CRay(QVector3D v3dOrigin, QVector3D v3dDirection);
    ~CRay(){}

    QVector3D StartPoint() const;

    QVector3D Direction() const;

private:
    QVector3D m_v3dOrigin;
    //normalized
    QVector3D m_v3dDirection;
};

#endif // CRAY_H
