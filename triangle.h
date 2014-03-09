#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector3df.h"
#include "vector"
#include "AABox.h"
#include "vertex.h"
#include "bezierpatch.h"

class CRay;
class CIntersactionInfo;
class CSubTriangle;

class CTriangle
{
public:
    CTriangle();
    CTriangle( const std::vector<CVertex> & aVertecis, int v1, int v2, int v3, int nTriangleId = -1);
    virtual ~CTriangle();

    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;

    bool IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, std::vector<CVector3DF>* aPointsToCheck = NULL, bool bDebug = false) const;
    static bool IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, std::vector<CVector3DF>* aPointsToCheck = NULL, bool bDebug = false);

    virtual bool Intersect(const CVector3DF& vStart, const CVector3DF& vEnd) const;

    const std::vector<int>& Vertices() const;
    CVertex& GetVertex(int i);
    CVertex& GetVertex(int i) const;
    const CVector3DF& AB() const;
    const CVector3DF& AC() const;
    const CVector3DF& Normal() const;
    const CVertex &A() const;
    const CVertex &B() const;
    const CVertex &C() const;

    void MakeBoundingBox();
    const CAABox& GetBoundingBox();

    void BuildBezierPoints();
    void Subdivide();
    void AddSubTriangle(CSubTriangle* subTriangle);
    CSubTriangle* GetSubTriangle(int n) const
    {
        return m_aSubTriangles[n];
    }
    int GetNumberOfSubTriangle() const
    {
        return m_aSubTriangles.size();
    }

    CBezierPatch& GetBezierPatch()  { return m_BezierPatch; }
protected:
    std::vector<int> m_aVertIndices;
    std::vector<CVertex>& m_aVertecis;

    //bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;

    CVector3DF m_vAB, m_vAC, m_vNormal;

    std::vector<CSubTriangle*> m_aSubTriangles;

    int m_nTriangleId;

    CBezierPatch m_BezierPatch;
};


#endif // TRIANGLE_H
