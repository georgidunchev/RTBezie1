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


CTriangle::CTriangle( const QVector<CVertex> &aVertecis, int v1, int v2, int v3)
    : m_aVertecis(aVertecis)
    , m_bHasBoundingBox(false)
{
    m_aVertIndices << v1 << v2 << v3;
    m_aAdditionalPoints.fill(QVector3D(0, 0, 0), 10);
    m_aAdditionalPoints[0] = m_aVertecis[m_aVertIndices[0]].GetPos();
    m_aAdditionalPoints[1] = m_aVertecis[m_aVertIndices[1]].GetPos();
    m_aAdditionalPoints[2] = m_aVertecis[m_aVertIndices[2]].GetPos();
    m_vAB = B().GetPos() - A().GetPos();
    m_vAC = C().GetPos() - A().GetPos();
    m_vNormal = CUtils::Cross(AB(), AC()).normalized();
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
    std::vector<CVertexInfo> aVertices;
    // Push the needed information for generating the additional points
    aVertices.push_back(CVertexInfo(Point(3,0),
				    Point(0,3), Point(2,1),
				    Point(0,0), Point(2,0),
				    A().Normal_Get()));

    aVertices.push_back(CVertexInfo(Point(0,3),
				    Point(0,0), Point(0,2),
				    Point(3,0), Point(1,2),
				    B().Normal_Get()));

    aVertices.push_back(CVertexInfo(Point(0,0),
				    Point(3,0), Point(1,0),
				    Point(0,3), Point(0,1),
				    C().Normal_Get()));

    unsigned int nSize = aVertices.size();
    for( unsigned int i = 0; i < nSize; ++i )
    {
	BuildBezierPoint(aVertices[i]);
    }
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
    CUtils::ProjectionOfVectorInPlane(o_vNew, i_vNormal);
}

QVector3D& CTriangle::Point(int a, int b)
{
    if( a == 3 && b == 0 )
    {
	return m_aAdditionalPoints[0];
    }
    else if( a == 0 && b == 3 )
    {
	return m_aAdditionalPoints[1];
    }
    else if( a == 0 && b == 0 )
    {
	return m_aAdditionalPoints[2];
    }
    else if( a == 2 && b == 1 )
    {
	return m_aAdditionalPoints[3];
    }
    else if( a == 2 && b == 0 )
    {
	return m_aAdditionalPoints[4];
    }
    else if( a == 0 && b == 2 )
    {
	return m_aAdditionalPoints[5];
    }
    else if( a == 1 && b == 2 )
    {
	return m_aAdditionalPoints[6];
    }
    else if( a == 1 && b == 0 )
    {
	return m_aAdditionalPoints[7];
    }
    else if( a == 0 && b == 1 )
    {
	return m_aAdditionalPoints[8];
    }
    else if( a == 1 && b == 1 )
    {
	return m_aAdditionalPoints[9];
    }
}

bool CTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo) const
{
//    qDebug()<<ray.Direction();
//    printf("%f %f %f",ray.Direction().x(),ray.Direction().y(),ray.Direction().z());

    //intersect stuff
    double lambda2;
    double lambda3;
    double closestdist = intersectionInfo.m_fDistance;

//    const Vector& a = AB();
//    const Vector& b = AC();
    QVector3D c = - (ray.Direction());
    QVector3D h = ray.StartPoint() - A().GetPos();
    /* 2. Solve the equation:
	     *
	     * A + lambda2 * AB + lambda3 * AC = ray.start + gamma * ray.dir
	     *
	     * which can be rearranged as:
	     * lambda2 * AB + lambda3 * AC - gamma * ray.dir = ray.start - A
	     *
	     * Which is a linear system of three rows and three unknowns, which we solve using Carmer's rule
	     */
    //
    // Find the determinant of the left part of the equation
    double Dcr = CUtils::Triple(AB(), AC(), c);
    // check for zero; if it is zero, then the triangle and the ray are parallel
    if (fabs(Dcr) < 1e-9)
	return false;
    // find the reciprocal of the determinant. We would use this quantity later in order
    // to multiply by rDcr instead of divide by Dcr (division is much slower)
    double rDcr = 1.0 / Dcr;
    // calculate `gamma' by substituting the right part of the equation in the third column of the matrix,
    // getting the determinant, and dividing by Dcr)
    double gamma = CUtils::Triple(AB(), AC(), h) * rDcr;
    // Is the intersection point behind us?  Is the intersection point worse than what we currently have?
    if (gamma <= 0 || gamma > closestdist)
	return false;
    lambda2 = CUtils::Triple(h, AC(), c) * rDcr;
    // Check if it is in range (barycentric coordinates)
    if (lambda2 < 0 || lambda2 > 1)
	return false;
    lambda3 = CUtils::Triple(AB(), h, c) * rDcr;

    // Calculate lambda3 and check if it is in range as well
    if (lambda3 < 0 || lambda3 > 1)
	return false;
    if (lambda2 + lambda3 > 1)
	return false;

    closestdist = gamma;
    intersectionInfo.m_fDistance = closestdist;
    intersectionInfo.m_vNormal = m_vNormal;
    intersectionInfo.m_vIntersectionPoint = CUtils::GetPointAtDistance(ray, closestdist);
    return true;

    intersectionInfo.m_fDistance = 1.f;
    return true;
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
