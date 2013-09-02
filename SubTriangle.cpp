#include "SubTriangle.h"
#include "triangle.h"
#include <intersactioninfo.h>
#include <ray.h>
#include <QVector3D>
#include <vector>
#include <qmath.h>
#include <QDebug>
#include <Utils.h>
#include <main.h>
#include <raytracer.h>
#include <cmesh.h>
#include <vector>
#include <settings.h>

CSubTriangle::CSubTriangle(CTriangle& triangle)
	: m_Parent(triangle)
	, m_vA(m_Parent.A().GetPos())
	, m_vB(m_Parent.B().GetPos())
	, m_vC(m_Parent.C().GetPos())
	, m_vABar(1.0f, 0.0f, 0.0f)
	, m_vBBar(0.0f, 1.0f, 0.0f)
	, m_vCBar(0.0f, 0.0f, 1.0f)	
	, m_nSavePos(0)
	, m_nSubdivisionLevel(1)
{}

CSubTriangle::CSubTriangle(	const QVector3D& vA,
							const QVector3D& vB,
							const QVector3D& vC,	
							const QVector3D& m_vABar,
							const QVector3D& m_vBBar,
							const QVector3D& m_vCBar,
							uint nSubdivisionLevel,
							CTriangle& Parent,
							uint nSavePos)
	: m_Parent(Parent)
	, m_vA(vA)
	, m_vB(vB)
	, m_vC(vC)
	, m_vABar(m_vABar)
	, m_vBBar(m_vBBar)
	, m_vCBar(m_vCBar)	
	, m_nSavePos(nSavePos)
	, m_nSubdivisionLevel(nSubdivisionLevel)
{

}

const QVector3D& CSubTriangle::GetVert(int i) const
{
	i %= 3;
	switch (i)
	{
	case 0:
		{
			return m_vA;
		}
	case 1:
		{
			return m_vB;
		}
	default:
		{
			return m_vC;
		}
	}
}

const QVector3D& CSubTriangle::GetVertBar(int i) const
{
		i %= 3;
	switch (i)
	{
	case 0:
		{
			return m_vABar;
		}
	case 1:
		{
			return m_vBBar;
		}
	default:
		{
			return m_vCBar;
		}
	}
}

void CSubTriangle::Subdivide()
{	
	if (m_nSubdivisionLevel > GetSettings()->GetNofSubdivisions())
	{
		//qDebug() << m_vABar << m_vBBar << m_vCBar;
		//qDebug()	<< m_nSavePos
		//			<< m_vA 
		//			<< m_vB 
		//			<< m_vC;

		return;
	}
	
	int nStartOfLongest;
	QVector3D vMidPointBar;
	QVector3D vMidPoint;

	GetDivision(nStartOfLongest, vMidPoint, vMidPointBar);
	
	//divide subtriangle by the longest edge
	const int nSecondPoint = (nStartOfLongest + 1) % 3;
	const int nThirdPoint = (nStartOfLongest + 2) % 3;

	const QVector3D& vertex3 = GetVert(nThirdPoint);
	const QVector3D& vertex3Bar = GetVertBar(nThirdPoint);

	// populate the second subtriangle first, because the current subtriangle will be overridden when the first subtriangle is saved
	const uint nNewSavePoint = m_nSavePos +
		CUtils::PowerOf2( static_cast<int>(GetSettings()->GetNofSubdivisions()) -
				  m_nSubdivisionLevel);
	CSubTriangle* pSecondSubTriangle = new CSubTriangle(vMidPoint, GetVert(nSecondPoint), vertex3
														, vMidPointBar, GetVertBar(nSecondPoint), vertex3Bar
														, m_nSubdivisionLevel + 1, m_Parent, nNewSavePoint);
	m_Parent.AddSubTriangle(pSecondSubTriangle);
	
	CSubTriangle* pFirstSubTriangle = new CSubTriangle(	GetVert(nStartOfLongest), vMidPoint, vertex3
														, GetVertBar(nStartOfLongest), vMidPointBar, vertex3Bar
														, m_nSubdivisionLevel + 1, m_Parent, m_nSavePos);
	m_Parent.AddSubTriangle(pFirstSubTriangle);	
}

void CSubTriangle::GetDivision(int& o_nStartOfLongest, QVector3D& o_vMidPoint, QVector3D& o_vMidPointBar) const
{	
	float fDistance = 0.0f;

	QVector3D vMidPointBar, vMidPoint;

	for (int i = 0; i < 3; i++)
	{
		const int j = (i + 1) % 3;

		vMidPointBar = (GetVertBar(i) + GetVertBar(j)) / 2.0f;
		vMidPoint = m_Parent.GetPointFromBarycentric(vMidPointBar);

		const float fNewDistance = (GetVert(i) - vMidPoint).lengthSquared();// this should work with bezier stuff

		if (fNewDistance > fDistance)
		{
			fDistance = fNewDistance;
			o_nStartOfLongest = i;
			o_vMidPoint = vMidPoint;
			o_vMidPointBar = vMidPointBar;
		}
	}
}

bool CSubTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
	bool bIntersect = CUtils::IntersectTriangle(ray, intersectionInfo, m_vA, m_vB, m_vC, m_vABar, m_vBBar, m_vCBar);
	if (bIntersect)
	{
		//Smoothed normal
		QVector3D vNormalA = intersectionInfo.u * m_Parent.A().Normal_Get();
		QVector3D vNormalB = intersectionInfo.v * m_Parent.B().Normal_Get();
		QVector3D vNormalC = intersectionInfo.w * m_Parent.C().Normal_Get();
		intersectionInfo.m_vNormal = vNormalA + vNormalB + vNormalC;
		intersectionInfo.m_vNormal.normalize();
	}
	return bIntersect;
}

void CSubTriangle::MakeBoundingBox()
{
	for (int i = 0; i < 3; i++)
	{
		m_BoundingBox.AddPoint(GetVert(i));
	}

	m_bHasBoundingBox = true;
}

const CAABox& CSubTriangle::GetBoundingBox()
{
	return m_BoundingBox;
}
