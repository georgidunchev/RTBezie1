#ifndef BEZIERTRIANGLE_H
#define BEZIERTRIANGLE_H

#include <QVector3D>
#include <QVector>
#include <AABox.h>
#include <triangle.h>

class CRay;
class CIntersactionInfo;

class CBezierTriangle : public CTriangle
{
public:
    CBezierTriangle();
    CBezierTriangle(int nMainTriangleID);

    bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
    bool Intersect(const QVector3D& vStart, const QVector3D& vEnd) const;

//    void MakeBoundingBox();
//    const CAABox& GetBoundingBox();

private:
//    QVector<int> m_aVertIndices;
//    QVector<QVector3D> m_aVertecis;

//    //bounding box
//    bool m_bHasBoundingBox;
//    CAABox m_BoundingBox;

//    QVector3D m_vAB, m_vAC, m_vNormal;
};

#endif // BEZIERTRIANGLE_H

