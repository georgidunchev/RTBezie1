#ifndef BEZIERTRIANGLE_H
#define BEZIERTRIANGLE_H

#include <QVector3D>
//#include <vector>
#include <AABox.h>
#include <triangle.h>

class CRay;
class CIntersactionInfo;

class CBezierTriangle : public CTriangle
{
public:
    CBezierTriangle();
    CBezierTriangle(int nMainTriangleID);

    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
    virtual bool Intersect(const QVector3D& vStart, const QVector3D& vEnd) const;

//    void MakeBoundingBox();
    //    const CAABox& GetBoundingBox();
private:
    bool intersectSimpleBezierTriangle(const CRay &ray, CIntersactionInfo &info, QVector3D &barCoord, unsigned int iterations) const;
    QVector3D Q20, Q02, Q11, Q10, Q01, Q00;

//    std::vector<int> m_aVertIndices;
//    std::vector<QVector3D> m_aVertecis;

//    //bounding box
//    bool m_bHasBoundingBox;
//    CAABox m_BoundingBox;

//    QVector3D m_vAB, m_vAC, m_vNormal;
};

#endif // BEZIERTRIANGLE_H

