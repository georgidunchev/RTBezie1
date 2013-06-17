#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QVector3D>
#include <vector>
#include <AABox.h>
#include <vertex.h>

class CRay;
class CIntersactionInfo;

struct CVertexInfo
{
    QVector3D* vMain;
    QVector3D* vEnd1;
    QVector3D* vEnd2;
    QVector3D* vNew1;
    QVector3D* vNew2;
    const QVector3D* vNormal;
    CVertexInfo()
    {};
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
    {};
};

class CTriangle
{
public:
    CTriangle();

    CTriangle( const std::vector<CVertex> & aVertecis, int v1, int v2, int v3);
	virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
	virtual bool IntersectFast(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
    virtual bool IntersectHighQuality(const CRay& ray, CIntersactionInfo& intersectionInfo ) const;
    virtual bool Intersect(const QVector3D& vStart, const QVector3D& vEnd) const;

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

    void BuildBezierPoints();

protected:
    std::vector<int> m_aVertIndices;
    std::vector<CVertex>& m_aVertecis;

    int GetIndex(int a, int b) const;
    QVector3D &Point(int a, int b);
    const QVector3D &GetPoint(int a, int b) const;

    void BuildBezierPoint(CVertexInfo& o_Info);
    void BuildBezierPoint(QVector3D& o_vNew,
			  const QVector3D& i_vMain,
			  const QVector3D& i_vEnd,
			  const QVector3D& i_vNormal);

    //bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;

    QVector3D m_vAB, m_vAC, m_vNormal;
    std::vector<QVector3D> m_aAdditionalPoints;

    bool intersectSimpleBezierTriangle(const CRay &ray, CIntersactionInfo &info, QVector3D &barCoord, unsigned int iterations) const;
    QVector3D Q30, Q03, Q21, Q12, Q20, Q02, Q11, Q10, Q01, Q00;
};


#endif // TRIANGLE_H
