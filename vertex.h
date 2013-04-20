#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

class CVertex
{
public:
    CVertex();
    CVertex(QVector3D vPos, int nId);
    const QVector3D &GetPos() const;

    void Normal_AddNormal(const QVector3D& vNormal);

    void Normal_Normalize();

    const QVector3D &Normal_Get() const;
    bool IsSmoothGeometryOn();

private:
    void ConstructorInit();
    QVector3D m_vPos;
    bool m_bSmoothGeometry;
    QVector3D m_vNormal;
    int m_nId;
};

#endif // VERTEX_H
