#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "QVector3D"
#include "vector"
#include "AABox.h"
#include "primitive.h"
#include "vertex.h"

class CRay;
class CIntersactionInfo;

class CPrimitive
{
public:
    CPrimitive() {}
    CPrimitive(const std::vector<CVertex> & aVertecis);

    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo ) const = 0;
    virtual bool Intersect(const QVector3D& vStart, const QVector3D& vEnd) const = 0;
    const std::vector<int>& Vertices() const;
    CVertex& GetVertex(int i);
    const QVector3D& AB() const;
    const QVector3D& AC() const;
    const QVector3D& Normal() const;
    const CVertex &A() const;
    const CVertex &B() const;
    const CVertex &C() const;

    void MakeBoundingBox();
    const CAABox& GetBoundingBox();

protected:
    std::vector<int> m_aVertIndices;
    std::vector<CVertex> m_aVertecis;

    //bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;

    QVector3D m_vAB, m_vAC, m_vNormal;
};

#endif // PRIMITIVE_H
