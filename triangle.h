#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>
#include <QVector>
#include <AABox.h>

class CRay;
class CIntersactionInfo;

class CTriangle
{
public:
    CTriangle() {}

    CTriangle( const QVector<QVector3D> & aVertecis, int v1, int v2, int v3);
    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
    virtual bool Intersect(const QVector3D& vStart, const QVector3D& vEnd) const;
    const QVector<int>& Vertices() const;
    const QVector3D& AB() const;
    const QVector3D& AC() const;
    const QVector3D& Normal() const;
    const QVector3D& A() const;
    const QVector3D& B() const;
    const QVector3D& C() const;

    void MakeBoundingBox();
    const CAABox& GetBoundingBox();

protected:
    QVector<int> m_aVertIndices;
    QVector<QVector3D> m_aVertecis;

    //bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;

    QVector3D m_vAB, m_vAC, m_vNormal;
};


#endif // TRIANGLE_H
