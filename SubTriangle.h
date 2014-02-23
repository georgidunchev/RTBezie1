#ifndef SUB_TRIANGLE_H
#define SUB_TRIANGLE_H

#include "vector3df.h"
#include "vector"
#include "AABox.h"
#include "vertex.h"
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
		 const CVector3DF& m_vABar,
		 const CVector3DF& m_vBBar,
		 const CVector3DF& m_vCBar,
		 uint nSubdivisionLevel,
		 CSubTriangle *Parent_SubTriangle,
		 uint nSavePos);
    CSubTriangle(CTriangle& triangle,
                 const CVector3DF& m_vABar,
                 const CVector3DF& m_vBBar,
                 const CVector3DF& m_vCBar);

    virtual ~CSubTriangle() {}

    void Subdivide();

    static bool IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, bool bDebug = false);
    virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;

    int GetSavePos() const { return m_nSavePos; }

    const CVector3DF& GetVert(int i) const;
    const CVector3DF& GetVertBar(int i) const;

    void MakeBoundingBox();
    const CAABox& GetBoundingBox();

    CTriangle& GetParent() const { return m_Parent; }
    CSubTriangle* GetParentSubTriangle() const { return m_pParent_SubTriangle; }

    CBezierPatch* GetBezierPatch()  { return m_pBezierPatch; }

    const CVector3DF GetParentBar(const CVector3DF& vLocalBar) const;
    //debug
    int m_nSubtriangleID;

private:

    void GetDivision(int& o_nStartOfLongest, CVector3DF& o_vMidPoint, CVector3DF& o_vMidPointBar) const;

private:
    CTriangle& m_Parent;
    CSubTriangle* m_pParent_SubTriangle;
    CVector3DF m_vABar;
    CVector3DF m_vBBar;
    CVector3DF m_vCBar;
    uint m_nSavePos;
    uint m_nSubdivisionLevel;

    CBezierPatch* m_pBezierPatch;

    //bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;
};

#endif // SUB_TRIANGLE_H
