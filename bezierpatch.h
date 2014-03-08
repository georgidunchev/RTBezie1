#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include "vector3df.h"
#include "QVector2D"
#include "vector"

class CRay;
class CIntersactionInfo;
class CTriangle;
class CSubTriangle;

struct CVertexInfo
{
    CVector3DF* vMain;
    CVector3DF* vEnd1;
    CVector3DF* vEnd2;
    CVector3DF* vNew1;
    CVector3DF* vNew2;
    const CVector3DF* vNormal;
    CVertexInfo() {}
    CVertexInfo(CVector3DF& pMain,
		CVector3DF& pEnd1,
		CVector3DF& pNew1,
		CVector3DF& pEnd2,
		CVector3DF& pNew2,
		const CVector3DF& pNormal)
	:vMain(&pMain)
	,vEnd1(&pEnd1)
	,vEnd2(&pEnd2)
	,vNew1(&pNew1)
	,vNew2(&pNew2)
	,vNormal(&pNormal)
    {}
};

class CBezierPatch
{
public:
    CBezierPatch(CTriangle &pParent);
    CBezierPatch(CSubTriangle* pParent);

    const CVector3DF GetPointFromBarycentric(const CVector3DF& vCoords) const;
    const CVector3DF GetPointFromBarycentric(const QVector2D& vCoords) const;
    const CVector3DF GetPointFromBarycentric(float u, float v) const;

    const CVector3DF GetdBu(float u, float v) const;
    const CVector3DF GetdBv(float u, float v) const;

    const CVector3DF GetSmoothedNormal(const CVector3DF& vCoords) const;
    const CVector3DF GetSubSurfSmoothedNormal(const CVector3DF& vCoords) const;

    // used for triangles from the mesh
    void BuildBezierPoints();
    // used for initial sub triangles created from triangle
    void BuildBezierPoints_InitialSub();
    // used for sub triangles
    void BuildBezierPoints_Sub(int nStartOfLongest, bool bFirst);

    void BuildBezierPoints_Sub2();

    void Subdivide();
//    void AddSubTriangle(CSubTriangle* subTriangle);
//    CSubTriangle* GetSubTriangle(int n) const
//    {
//	return m_aSubTriangles[n];
//    };

    bool IntersectHighQuality(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;
    bool IntersectHighQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, const CSubTriangle &SubTriangle, bool bDebug) const;

    bool IntersectLowQuality(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;

    bool intersect(const CRay &ray,
		   CIntersactionInfo &info,
		   CVector3DF barCoord,
		   unsigned int iterations,
		   bool bDebug = false) const;



    int GetIndex(int a, int b) const;
    CVector3DF &Point(int a, int b);
    CVector3DF &Point(int idx);
    const CVector3DF &GetPoint(int a, int b) const;
    const CVector3DF &GetPoint(QVector2D bar) const;

private:

    //used internally
    void BuildBezierPoints_Internal();

    void BuildBezierPoint(CVertexInfo& o_Info);
    void BuildBezierPoint(CVector3DF& o_vNew,
			  const CVector3DF& i_vMain,
			  const CVector3DF& i_vEnd,
			  const CVector3DF& i_vNormal);

    void Reverse(std::vector<int>& io_c, std::vector<int>& io_d);

    CVector3DF CalculateBezierPoint(int u, int v, int w) const;

    CTriangle& m_Parent_Triangle;
    CSubTriangle* m_pParent_SubTriangle;

    std::vector<CVector3DF> m_aAdditionalPoints;

    CVector3DF Q30, Q03, Q21, Q12, Q20, Q02, Q11, Q10, Q01, Q00;
};

#endif // BEZIERPATCH_H
