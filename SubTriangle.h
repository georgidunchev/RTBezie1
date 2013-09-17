#ifndef SUB_TRIANGLE_H
#define SUB_TRIANGLE_H

#include <QVector3D>
#include <vector>
#include <AABox.h>
#include <vertex.h>
#include "bezierpatch.h"

class CRay;
class CIntersactionInfo;
class CTriangle;

class CSubTriangle
{
public:
    CSubTriangle(CTriangle& triangle);
    CSubTriangle(int nStartOfLongest,
		 bool bFirst,
		 const QVector3D& m_vABar,
		 const QVector3D& m_vBBar,
		 const QVector3D& m_vCBar,
		 uint nSubdivisionLevel,
		 CSubTriangle *Parent_SubTriangle,
		 uint nSavePos);

    virtual ~CSubTriangle() {}

    void Subdivide();

    static bool IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, bool bDebug = false);
    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;

    const int GetSavePos() const { return m_nSavePos; }

    const QVector3D& GetVert(int i) const;
    const QVector3D& GetVertBar(int i) const;

    void MakeBoundingBox();
    const CAABox& GetBoundingBox();

    CTriangle& GetParent() const { return m_Parent; }
    CSubTriangle* GetParentSubTriangle() const { return m_pParent_SubTriangle; }

    CBezierPatch* GetBezierPatch()  { return m_pBezierPatch; }

    //debug
    int m_nSubtriangleID;

private:

    void GetDivision(int& o_nStartOfLongest, QVector3D& o_vMidPoint, QVector3D& o_vMidPointBar) const;

private:
    CTriangle& m_Parent;
    CSubTriangle* m_pParent_SubTriangle;
    QVector3D m_vABar;
    QVector3D m_vBBar;
    QVector3D m_vCBar;
    uint m_nSavePos;
    uint m_nSubdivisionLevel;

    CBezierPatch* m_pBezierPatch;

    //bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;
};

#endif // SUB_TRIANGLE_H
