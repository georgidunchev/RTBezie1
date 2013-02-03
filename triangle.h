#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>
#include <QVector>
class CRay;
class CIntersactionInfo;

class CTriangle
{
public:
    CTriangle() {}

    CTriangle( const QVector<QVector3D> & aVertecis, int v1, int v2, int v3);
    bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
    const QVector<int>& Vertices() const;
    const QVector3D& AB() const;
    const QVector3D& AC() const;
    const QVector3D& Normal() const;
    const QVector3D& A() const;
    const QVector3D& B() const;
    const QVector3D& C() const;

    void MakeBoundingBox();

private:
    QVector<int> m_aVertIndices;
    QVector<QVector3D> m_aVertecis;

    //bounding box
    bool m_bHasBoundingBox;
    QVector3D m_vMinVertex;
    QVector3D m_vMaxVertex;

    QVector3D m_vAB, m_vAC, m_vNormal;
};


#endif // TRIANGLE_H
