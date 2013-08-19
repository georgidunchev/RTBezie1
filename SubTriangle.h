#ifndef SUB_TRIANGLE_H
#define SUB_TRIANGLE_H

#include <QVector3D>
#include <vector>
#include <AABox.h>
#include <vertex.h>

class CRay;
class CIntersactionInfo;
class CTriangle;

class CSubTriangle
{
public:
	CSubTriangle(CTriangle& triangle);
	CSubTriangle(const QVector3D& vA,
				 const QVector3D& vB,
				 const QVector3D& vC,
				 const QVector3D& m_vABar,
				 const QVector3D& m_vBBar,
				 const QVector3D& m_vCBar,
				 int nSubdivisionLevel,
				 CTriangle& Parent,
				 int nSavePos);
	
	virtual ~CSubTriangle() {};

	void Subdivide();
	
	virtual bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false) const;
	
	const int GetSavePos() const { return m_nSavePos; }
		
	const QVector3D& GetVert(int i) const;
	const QVector3D& GetVertBar(int i) const;

	void MakeBoundingBox();
    const CAABox& GetBoundingBox();

	CTriangle& GetParent() const { return m_Parent; };

	//debug
	int m_nSubtriangleID;

private:

	void GetDivision(int& o_nStartOfLongest, QVector3D& o_vMidPoint, QVector3D& o_vMidPointBar) const;

private:
	CTriangle& m_Parent;
	QVector3D m_vA, m_vABar;
	QVector3D m_vB, m_vBBar;
	QVector3D m_vC, m_vCBar;
	int m_nSavePos;
	int m_nSubdivisionLevel;

	//bounding box
    bool m_bHasBoundingBox;
    CAABox m_BoundingBox;
};

#endif // SUB_TRIANGLE_H
