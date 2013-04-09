#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

class CVertex
{
public:
    CVertex();
    CVertex(QVector3D vPos);
    const QVector3D &GetPos() const;

    void Normal_AddNormal(const QVector3D& vNormal);

    void Normal_Normalize();

private:
    void ConstructorInit();
    QVector3D m_vPos;
    bool m_bSmoothGeometry;
    QVector3D m_vNormal;
};

#endif // VERTEX_H