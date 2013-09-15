#ifndef BEZIERPATCH_H
#define BEZIERPATCH_H

#include <QVector3D>
#include <QVector2D>
#include <vector>

class CRay;
class CIntersactionInfo;
class CTriangle;
class CSubTriangle;

struct CVertexInfo
{
    QVector3D* vMain;
    QVector3D* vEnd1;
    QVector3D* vEnd2;
    QVector3D* vNew1;
    QVector3D* vNew2;
    const QVector3D* vNormal;
    CVertexInfo() {}
    CVertexInfo(QVector3D& pMain,
		QVector3D& pEnd1,
		QVector3D& pNew1,
		QVector3D& pEnd2,
		QVector3D& pNew2,
		const QVector3D& pNormal)
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
    CBezierPatch(CTriangle* pParent);
    CBezierPatch(CSubTriangle* pParent);

    const QVector3D GetPointFromBarycentric(const QVector3D& vCoords);
    const QVector3D GetPointFromBarycentric(const QVector2D& vCoords);
    const QVector3D GetPointFromBarycentric(float u, float v);

    void BuildBezierPoints();
    void Subdivide();
//    void AddSubTriangle(CSubTriangle* subTriangle);
//    CSubTriangle* GetSubTriangle(int n) const
//    {
//	return m_aSubTriangles[n];
//    };

    bool intersect(const CRay &ray,
		   CIntersactionInfo &info,
		   QVector3D &barCoord,
		   unsigned int iterations,
		   bool bDebug = false) const;

private:
    int GetIndex(int a, int b) const;
    QVector3D &Point(int a, int b);
    const QVector3D &GetPoint(int a, int b) const;   

    void BuildBezierPoint(CVertexInfo& o_Info);
    void BuildBezierPoint(QVector3D& o_vNew,
			  const QVector3D& i_vMain,
			  const QVector3D& i_vEnd,
			  const QVector3D& i_vNormal);

    CTriangle* m_pParent_Triangle;
    CSubTriangle* m_pParent_SubTriangle;

    std::vector<QVector3D> m_aAdditionalPoints;

    QVector3D Q30, Q03, Q21, Q12, Q20, Q02, Q11, Q10, Q01, Q00;
};

#endif // BEZIERPATCH_H
