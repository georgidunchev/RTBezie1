#include "triangle.h"
#include <intersactioninfo.h>
#include <ray.h>
#include <QVector3D>
#include <QVector2D>
#include <vector>
#include <qmath.h>
#include <QDebug>
#include <Utils.h>
#include <main.h>
#include <raytracer.h>
#include <cmesh.h>
#include <vector>
#include <settings.h>
#include <SubTriangle.h>
#include <color.h>


CTriangle::CTriangle()
    : m_aVertecis(GetRaytracer()->GetMesh().Vertices())
    , m_BezierPatch(*this)
{}

CTriangle::CTriangle( const std::vector<CVertex> &aVertecis, int v1, int v2, int v3, int nTriangleId)
    : m_aVertecis(GetRaytracer()->GetMesh().Vertices())
    , m_bHasBoundingBox(false)
    , m_nTriangleId(nTriangleId)
    , m_BezierPatch(*this)
{
    m_aVertIndices.push_back(v1);
    m_aVertIndices.push_back(v2);
    m_aVertIndices.push_back(v3);

    m_vAB = B().GetPos() - A().GetPos();
    m_vAC = C().GetPos() - A().GetPos();
    CUtils::Normal(m_vNormal, AB(), AC());
}

CTriangle::~CTriangle()
{
    m_aSubTriangles.clear();
}

const std::vector<int> &CTriangle::Vertices() const
{
    return m_aVertIndices;
}

CVertex& CTriangle::GetVertex(int i)
{
    return m_aVertecis[Vertices()[i]];
}

CVertex& CTriangle::GetVertex(int i) const
{
    return m_aVertecis[Vertices()[i]];
}
const QVector3D &CTriangle::AB() const
{
    return m_vAB;
}

const QVector3D &CTriangle::AC() const
{
    return m_vAC;
}

const QVector3D &CTriangle::Normal() const
{
    return m_vNormal;
}

const CVertex &CTriangle::A() const
{
    return m_aVertecis[m_aVertIndices[0]];
}

const CVertex &CTriangle::B() const
{
    return m_aVertecis[m_aVertIndices[1]];
}

const CVertex &CTriangle::C() const
{
    return m_aVertecis[m_aVertIndices[2]];
}

void CTriangle::MakeBoundingBox()
{
    int nSize = m_aSubTriangles.size();

    for(int i = 0; i < nSize; ++i)
    {
	m_aSubTriangles[i]->MakeBoundingBox();
	m_BoundingBox.AddPoint(m_aSubTriangles[i]->GetBoundingBox().GetMinVertex());
	m_BoundingBox.AddPoint(m_aSubTriangles[i]->GetBoundingBox().GetMaxVertex());
    }
    //    qDebug()<<"min"<<m_BoundingBox.GetMinVertex()<<"max"<<m_BoundingBox.GetMaxVertex();
    m_bHasBoundingBox = true;
}

const CAABox& CTriangle::GetBoundingBox()
{
    return m_BoundingBox;
}

void CTriangle::BuildBezierPoints()
{
    m_BezierPatch.BuildBezierPoints();   
}

void CTriangle::Subdivide()
{
    m_aSubTriangles.resize(CUtils::PowerOf2(GetSettings()->GetNofSubdivisions()));

    m_aSubTriangles[0] = new CSubTriangle(*this);
    m_aSubTriangles[0]->Subdivide();

    int nSize = m_aSubTriangles.size();
    for (int i = 0; i < nSize; ++i)
    {
	m_aSubTriangles[i]->m_nSubtriangleID = i;
    }
}

void CTriangle::AddSubTriangle(CSubTriangle* subTriangle)
{
    int nSavePos = subTriangle->GetSavePos();
    if (m_aSubTriangles[nSavePos])
    {
	delete m_aSubTriangles[nSavePos];
    }
    m_aSubTriangles[nSavePos] = subTriangle;
    m_aSubTriangles[nSavePos]->Subdivide();
}

bool CTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, bool bDebug)
{
    if (intersectionInfo.m_bHighQuality)
    {
	return CTriangle::IntersectHighQuality(ray, intersectionInfo, aSubTriangles, bDebug);
    }
    else
    {
	//return GetRaytracer()->GetMesh().IntersectKDTree(ray, intersectionInfo, bDebug);

	return IntersectSubdevidedTriangles(ray, intersectionInfo, aSubTriangles, NULL, bDebug);
	//return IntersectFast(ray, intersectionInfo);
    }
}
bool CTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
    return CTriangle::Intersect(ray, intersectionInfo, m_aSubTriangles, bDebug);
}
bool CTriangle::IntersectHighQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, bool bDebug)
{
    if (CTriangle::IntersectSubdevidedTriangles(ray, intersectionInfo, aSubTriangles, NULL, bDebug))
    {
	return CTriangle::IntersectHighQuality(ray, intersectionInfo, bDebug);
    }
    return false;
}

bool CTriangle::IntersectHighQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug)
{
    std::vector<QVector3D> aPointsToCheck;
    CSubTriangle& SubTriangle = * intersectionInfo.pSubTriangle;

    aPointsToCheck.push_back(intersectionInfo.m_vBarCoordsLocal);
    for (int j = 0; j < 3; j++)
    {
	aPointsToCheck.push_back(SubTriangle.GetVertBar(j));
    }

    float fU = intersectionInfo.m_vBarCoordsLocal.x();
    float fV = intersectionInfo.m_vBarCoordsLocal.y();

    float closestdist = k_fMAX;
    intersectionInfo.m_fDistance = k_fMAX;

    bool bezierFast = false;
    if (!bezierFast)
    {
	int nSize = aPointsToCheck.size();
	if (nSize > 0)
	{
	    for (int i = 0; i < nSize; i++)
	    {
		if (intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, aPointsToCheck[i], 15, bDebug))
		{
		    closestdist = intersectionInfo.m_fDistance;
		    return true;
		}

	    }
	    return false;
	}
	else
	{
	    QVector3D res = QVector3D(fU, fV, 0);
	    if (intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, 5, bDebug))
	    {
		closestdist = intersectionInfo.m_fDistance;
		return true;
	    }
	    else
	    {
		return false;
	    }
	}
    }
    else
    {
	int iterations = 5;
	QVector3D res = QVector3D(1.0/3.0, 1.0/3.0, 0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
	res = QVector3D(1.0/6.0, 1.0/6.0, 0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
	res = QVector3D(2.0/3.0, 1.0/6.0, 0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
	res = QVector3D(1.0/6.0, 2.0/3.0 ,0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );

	closestdist = intersectionInfo.m_fDistance;
    }

    return fabs(closestdist - k_fMAX) > k_fMIN;
}

bool CTriangle::intersectSimpleBezierTriangle(const CRay &ray, CIntersactionInfo &info, CSubTriangle& SubTriangle, QVector3D &barCoord, unsigned int iterations, bool bDebug)
{
    return SubTriangle.GetParent().GetBezierPatch().intersect(ray, info, barCoord, iterations, bDebug);
}

//not used
bool CTriangle::IntersectFast(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
    for (int i = 0; i < 9; i++)
    {
//	if (GetBezierPatch().intersect(ray, intersectionInfo, i, bDebug))
	{
	    return true;
	}
    }

    return false;
}

bool CTriangle::IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, std::vector<QVector3D>* aPointsToCheck, bool bDebug) const
{
    return IntersectSubdevidedTriangles(ray, intersectionInfo, m_aSubTriangles, aPointsToCheck, bDebug);
}

bool CTriangle::IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, std::vector<QVector3D>* aPointsToCheck, bool bDebug)
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

bool CTriangle::Intersect(const QVector3D &vStart, const QVector3D &vEnd) const
{
    CIntersactionInfo Intersection;
    CRay Ray(vStart, vEnd);
    if (!Intersect(Ray, Intersection))
    {
	return false;
    }

    float fLength = (vEnd - vStart).length();
    if (Intersection.m_fDistance < fLength)
    {
	return true;
    }
    return false;
}

