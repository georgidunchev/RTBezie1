#include "triangle.h"
#include <intersactioninfo.h>
#include <ray.h>
#include <QVector3D>
#include <QVector>
#include <qmath.h>
#include <QDebug>
#include <Utils.h>
#include <main.h>
#include <raytracer.h>
#include <cmesh.h>
#include <vector>


CTriangle::CTriangle()
    : m_aVertecis(GetRaytracer()->GetMesh().Vertices())
{}

CTriangle::CTriangle( const QVector<CVertex> &aVertecis, int v1, int v2, int v3)
    : m_aVertecis(GetRaytracer()->GetMesh().Vertices())
    , m_bHasBoundingBox(false)
{
    m_aVertIndices << v1 << v2 << v3;
    m_aAdditionalPoints.fill(QVector3D(0, 0, 0), 10);
    m_aAdditionalPoints[0] = m_aVertecis[m_aVertIndices[0]].GetPos();
    m_aAdditionalPoints[1] = m_aVertecis[m_aVertIndices[1]].GetPos();
    m_aAdditionalPoints[2] = m_aVertecis[m_aVertIndices[2]].GetPos();
    m_vAB = B().GetPos() - A().GetPos();
    m_vAC = C().GetPos() - A().GetPos();
    CUtils::Normal(m_vNormal, AB(), AC());
}


const QVector<int> &CTriangle::Vertices() const
{
    return m_aVertIndices;
}

CVertex& CTriangle::GetVertex(int i)
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
    for(int i = 0; i < m_aVertIndices.size(); ++i)
    {
	m_BoundingBox.AddPoint(m_aVertecis[m_aVertIndices[i]]);
//	qDebug()<<m_aVertecis[m_aVertIndices[i]];
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
    QVector<CVertexInfo> aVertices;
    // Push the needed information for generating the additional points
    aVertices.append(CVertexInfo(Point(3,0),
				    Point(0,3), Point(2,1),
				    Point(0,0), Point(2,0),
				    A().Normal_Get()));

//    aVertices.append(CVertexInfo(Point(0,3),
//				    Point(0,0), Point(0,2),
//				    Point(3,0), Point(1,2),
//				    B().Normal_Get()));

//    aVertices.append(CVertexInfo(Point(0,0),
//				    Point(3,0), Point(1,0),
//				    Point(0,3), Point(0,1),
//				    C().Normal_Get()));

	Point(0,2) = Point(0,3);
    	Point(1,2) = Point(0,3);

	Point(1,0) = Point(0,0);
	Point(0,1) = Point(0,0);

    //Generate the new bezier points
    unsigned int nSize = aVertices.size();
    for( unsigned int i = 0; i < nSize; ++i )
    {
	BuildBezierPoint(aVertices[i]);
    }

    //Generate last point
    CUtils::TriangleCentre( Point(1,1),
			    Point(2,0), Point(1,2), Point(0,1));
}

void CTriangle::BuildBezierPoint(CVertexInfo &o_Info)
{
    BuildBezierPoint(*o_Info.vNew1, *o_Info.vMain, *o_Info.vEnd1, *o_Info.vNormal);
    BuildBezierPoint(*o_Info.vNew2, *o_Info.vMain, *o_Info.vEnd2, *o_Info.vNormal);
}

void CTriangle::BuildBezierPoint(QVector3D &o_vNew,
				 const QVector3D &i_vMain,
				 const QVector3D &i_vEnd,
				 const QVector3D &i_vNormal)
{
    o_vNew = i_vEnd - i_vMain;
    o_vNew *= 0.25f;
    o_vNew = CUtils::ProjectionOfVectorInPlane(o_vNew, i_vNormal);
}

int CTriangle::GetIndex(int a, int b) const
{
    if( a == 3 && b == 0 )
    {
	return 0;
    }
    else if( a == 0 && b == 3 )
    {
	return 1;
    }
    else if( a == 0 && b == 0 )
    {
	return 2;
    }
    else if( a == 2 && b == 1 )
    {
	return 3;
    }
    else if( a == 2 && b == 0 )
    {
	return 4;
    }
    else if( a == 0 && b == 2 )
    {
	return 5;
    }
    else if( a == 1 && b == 2 )
    {
	return 6;
    }
    else if( a == 1 && b == 0 )
    {
	return 7;
    }
    else if( a == 0 && b == 1 )
    {
	return 8;
    }
    else if( a == 1 && b == 1 )
    {
	return 9;
    }
}

QVector3D& CTriangle::Point(int a, int b)
{
    return m_aAdditionalPoints[GetIndex(a,b)];
}

const QVector3D &CTriangle::GetPoint(int a, int b) const
{
    return m_aAdditionalPoints[GetIndex(a,b)];
}

bool CTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo) const
{
    if ( CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(3,0), GetPoint(2,1), GetPoint(2,0))
	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(2,1), GetPoint(1,2), GetPoint(1,1))
	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(1,2), GetPoint(0,3), GetPoint(0,2))
	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(2,0), GetPoint(1,1), GetPoint(1,0))
	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(1,1), GetPoint(0,2), GetPoint(0,1))
	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(1,0), GetPoint(0,1), GetPoint(0,0)) )
//    if ( CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(3,0), GetPoint(0,3), GetPoint(0,0)))
    {
	return true;
    }
    else
    {
	return false;
    }
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
