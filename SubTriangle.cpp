#include "SubTriangle.h"
#include "triangle.h"
#include "bezierpatch.h"
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
    , m_pParent_SubTriangle(NULL)
    , m_vABar(1.0f, 0.0f, 0.0f)
    , m_vBBar(0.0f, 1.0f, 0.0f)
    , m_vCBar(0.0f, 0.0f, 1.0f)
    , m_nSavePos(0)
    , m_nSubdivisionLevel(1)
{
    m_pBezierPatch = new CBezierPatch(this);
    m_pBezierPatch->BuildBezierPoints_InitialSub();
}

CSubTriangle::CSubTriangle(int nStartOfLongest,
	     bool bFirst,
	     const QVector3D &m_vABar,
	     const QVector3D &m_vBBar,
	     const QVector3D &m_vCBar,
	     uint nSubdivisionLevel, CSubTriangle *Parent_SubTriangle, uint nSavePos)
    : m_Parent(Parent_SubTriangle->GetParent())
    , m_pParent_SubTriangle(Parent_SubTriangle)
    , m_vABar(m_vABar)
    , m_vBBar(m_vBBar)
    , m_vCBar(m_vCBar)
    , m_nSavePos(nSavePos)
    , m_nSubdivisionLevel(nSubdivisionLevel)
{
    m_pBezierPatch = new CBezierPatch(this);
    m_pBezierPatch->BuildBezierPoints_Sub(nStartOfLongest, bFirst);

//    qDebug() << "1" << m_Parent.GetBezierPatch().GetPointFromBarycentric(m_vABar)
//	     << m_Parent.GetBezierPatch().GetPointFromBarycentric(m_vBBar)
//	     << m_Parent.GetBezierPatch().GetPointFromBarycentric(m_vCBar)
//	     << "Longest" << nStartOfLongest;
//    qDebug() << "2" <<m_pBezierPatch->GetPoint(3,0) << m_pBezierPatch->GetPoint(0,3) << m_pBezierPatch->GetPoint(0,0) << "Longest" << nStartOfLongest;
}

const QVector3D& CSubTriangle::GetVert(int i) const
{
    i %= 3;
    switch (i)
    {
    case 0:
    {
//	return m_vA;
	return m_pBezierPatch->GetPoint(3,0);
    }
    case 1:
    {
//	return m_vB;
	return m_pBezierPatch->GetPoint(0,3);
    }
    default:
    {
//	return m_vC;
	return m_pBezierPatch->GetPoint(0,0);
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
	//qDebug() << m_nSavePos << m_vA << m_vB << m_vC;
	return;
    }

    int nStartOfLongest;
    QVector3D vMidPointBar;
    QVector3D vMidPoint;

    GetDivision(nStartOfLongest, vMidPoint, vMidPointBar);

    // populate the second subtriangle first, because the current subtriangle will be overridden when the first subtriangle is saved
    const uint nNewSavePoint = m_nSavePos + CUtils::PowerOf2( static_cast<int>(GetSettings()->GetNofSubdivisions()) - m_nSubdivisionLevel);
    QVector3D a[3] = {GetVertBar(0), GetVertBar(1), GetVertBar(2)};
    a[nStartOfLongest] = vMidPointBar;
    CSubTriangle* pSecondSubTriangle = new CSubTriangle(nStartOfLongest, false
							, a[0], a[1], a[2]
							, m_nSubdivisionLevel + 1, this, nNewSavePoint);
    m_Parent.AddSubTriangle(pSecondSubTriangle);

    QVector3D b[3] = {GetVertBar(0), GetVertBar(1), GetVertBar(2)};
    b[(nStartOfLongest+1)%3] = vMidPointBar;
    CSubTriangle* pFirstSubTriangle = new CSubTriangle(	nStartOfLongest, true
							, b[0], b[1], b[2]
							, m_nSubdivisionLevel + 1, this, m_nSavePos);
    m_Parent.AddSubTriangle(pFirstSubTriangle);
}

void CSubTriangle::GetDivision(int& o_nStartOfLongest, QVector3D& o_vMidPoint, QVector3D& o_vMidPointBar) const
{	
    //get parent bezier patch
//    const CBezierPatch& ParentBezierPatch = m_pParent_SubTriangle ? m_pParent_SubTriangle->GetBezierPatch() : m_Parent.GetBezierPatch();
    float fDistance = 0.0f;

    QVector3D vMidPointBar, vMidPoint;

    for (int i = 0; i < 3; i++)
    {
	const int j = (i + 1) % 3;

	vMidPointBar = (GetVertBar(i) + GetVertBar(j)) / 2.0f;
	vMidPoint = m_Parent.GetBezierPatch().GetPointFromBarycentric(vMidPointBar);

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

bool CSubTriangle::IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, bool bDebug)
{
    int nSize = aSubTriangles.size();
    float fModifier = 8.0f / static_cast<float>(nSize);

    bool bIntersected = false;

    for (int i = 0; i < nSize; i++)
    {
	CIntersactionInfo intersectionInfoLocal(intersectionInfo);
	if( aSubTriangles[i]->Intersect(ray, intersectionInfoLocal,bDebug) )
	{
	    int nId = aSubTriangles[i]->m_nSubtriangleID;

	    if (false)
	    {
		int nSubtriangleId = aSubTriangles[i]->m_nSubtriangleID;
		bool bR = ((nSubtriangleId / 4) > 0);
		bool bG = (((nSubtriangleId % 4) / 2) > 0);
		bool bB = (((nSubtriangleId % 4) % 2) > 0);

		float fR = bR ? fModifier * static_cast<float>(nSubtriangleId) * 0.125f : 0.0f;
		float fG = bG ? fModifier * static_cast<float>(nSubtriangleId) * 0.125f : 0.0f;
		float fB = bB ? fModifier * static_cast<float>(nSubtriangleId) * 0.125f : 0.0f;

		intersectionInfoLocal.color = CColor(fR, fG, fB);
	    }

	    intersectionInfoLocal.pSubTriangle = aSubTriangles[i];

	    intersectionInfoLocal.m_nSubTriangleId = nId;

	    if (intersectionInfoLocal.m_fDistance < intersectionInfo.m_fDistance)
	    {
		intersectionInfo = intersectionInfoLocal;
		bIntersected = true;
	    }
	}
    }
    return bIntersected;
}

bool CSubTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
    bool bIntersect = CUtils::IntersectTriangle(ray, intersectionInfo, GetVert(0), GetVert(1), GetVert(2), m_vABar, m_vBBar, m_vCBar);
//    bool bIntersect = m_pBezierPatch->intersect(ray, intersectionInfo, bDebug);
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
