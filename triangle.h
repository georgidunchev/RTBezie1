#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>
#include <vector>
#include <AABox.h>
#include <vertex.h>
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

    static bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, bool bDebug = false);
    static bool IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, std::vector<QVector3D>* aPointsToCheck = NULL, bool bDebug = false);
    static bool IntersectHighQuality(const CRay& ray, CIntersactionInfo& intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, bool bDebug = false);
    static bool IntersectHighQuality(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false);
    static bool intersectSimpleBezierTriangle(const CRay &ray, CIntersactionInfo &info, CSubTriangle& SubTriangle, QVector3D &barCoord, unsigned int iterations, bool bDebug = false);


    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;
    bool IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, std::vector<QVector3D>* aPointsToCheck = NULL, bool bDebug = false) const;

    //not used///////////////////////////////////////////////////////////////////
    virtual bool IntersectFast(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;
    bool IntersectBezierSubTriangle(const CRay& ray, CIntersactionInfo& intersectionInfo, int i_nTriangleId, bool bDebug = false) const;
    /////////////////////////////////////////////////////////////////////////



    virtual bool Intersect(const QVector3D& vStart, const QVector3D& vEnd) const;

    const std::vector<int>& Vertices() const;
    CVertex& GetVertex(int i);
    CVertex& GetVertex(int i) const;
    const QVector3D& AB() const;
    const QVector3D& AC() const;
    const QVector3D& Normal() const;
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
    };

    CBezierPatch& GetBezierPatch()  { return m_BezierPatch; };
protected:
    std::vector<int> m_aVertIndices;
    std::vector<CVertex>& m_aVertecis;

    //bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;

    QVector3D m_vAB, m_vAC, m_vNormal;

    std::vector<CSubTriangle*> m_aSubTriangles;

    int m_nTriangleId;

    CBezierPatch m_BezierPatch;
};


#endif // TRIANGLE_H
