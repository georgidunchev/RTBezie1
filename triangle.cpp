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
#include <settings.h>

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

    aVertices.append(CVertexInfo(Point(0,3),
				    Point(0,0), Point(0,2),
				    Point(3,0), Point(1,2),
				    B().Normal_Get()));

    aVertices.append(CVertexInfo(Point(0,0),
				    Point(3,0), Point(1,0),
				    Point(0,3), Point(0,1),
				    C().Normal_Get()));

//	Point(0,2) = Point(0,3);
//    	Point(1,2) = Point(0,3);

//	Point(1,0) = Point(0,0);
//	Point(0,1) = Point(0,0);

    //Generate the new bezier points
    unsigned int nSize = aVertices.size();
    for( unsigned int i = 0; i < nSize; ++i )
    {
	BuildBezierPoint(aVertices[i]);
    }

    //Generate last point
    CUtils::TriangleCentre( Point(1,1),
			    Point(2,0), Point(1,2), Point(0,1));

//    QVector3D vE = Point(2,1) + Point(1,2) + Point(0,2) + Point(0,1) + Point(1,0) + Point(2,0);
//    vE /= 6;
//    QVector3D vV = Point(0,0) + Point(3,0) + Point(0,3);

//    Point(1,1) = vE + (vE - vV)*0.5f;

    Q30 = Point(3,0) - Point(0,0) - 3 * Point(2,0) + 3 * Point(1,0);
    Q03 = Point(0,3) - Point(0,0) - 3 * Point(0,2) + 3 * Point(0,1);
    Q21 = 3 * Point(2,1) - 3 * Point(0,0) - 3 * Point(2,0) + 6 * Point(1,0) + 3 * Point(0,1) - 6 * Point(1,1);
    Q12 = 3 * Point(1,2) - 3 * Point(0,0) - 3 * Point(0,2) + 3 * Point(1,0) + 6 * Point(0,1) - 6 * Point(1,1);
    Q20 = 3*Point(0,0) + 3*Point(2,0) - 6*Point(1,0);
    Q02 = 3*Point(0,0) + 3*Point(0,2) - 6*Point(0,1);
    Q11 = 6*Point(0,0) - 6*Point(1,0) - 6*Point(0,1) + 6*Point(1,1);
    Q10 = 3*Point(1,0) - 3*Point(0,0);
    Q01 = 3*Point(0,1) - 3*Point(0,0);
    Q00 = Point(0,0);
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
    o_vNew *= 0.3333f;
    o_vNew = CUtils::ProjectionOfVectorInPlane(o_vNew, i_vNormal);
    o_vNew += i_vMain;
//    o_vNew = (i_vEnd - i_vMain);
//    float fDot = CUtils::Dot(o_vNew, i_vNormal);
//    o_vNew = 2 * i_vMain + i_vEnd - fDot * i_vNormal;
//    o_vNew *= 0.3333f;
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
//	if ( CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(3,0), GetPoint(2,1), GetPoint(2,0))
//    	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(2,1), GetPoint(1,2), GetPoint(1,1))
//    	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(1,2), GetPoint(0,3), GetPoint(0,2))
//    	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(2,0), GetPoint(1,1), GetPoint(1,0))
//    	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(1,1), GetPoint(0,2), GetPoint(0,1))
//    	 || CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(1,0), GetPoint(0,1), GetPoint(0,0)) )
//    //    if ( CUtils::IntersectTriangle(ray, intersectionInfo, GetPoint(3,0), GetPoint(0,3), GetPoint(0,0)))
//	{
//    	return true;
//	}
//	else
//	{
//    	return false;
//	}

    float closestdist = k_fMAX;
    intersectionInfo.m_fDistance = k_fMAX;

    bool bezierFast = false;
    if(!bezierFast)
    {
	QVector3D res = QVector3D(1.0/3.0, 1.0/3.0, 0);
	if(intersectSimpleBezierTriangle(ray, intersectionInfo, res, 4 ))
	{
	    closestdist = intersectionInfo.m_fDistance;
	    return true;
	}
	else
	{
	    return false;
	}
    }
    else
    {
	int iterations = 15;
	QVector3D res = QVector3D(1.0/3.0, 1.0/3.0, 0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, res, iterations );
	res = QVector3D(1.0/6.0, 1.0/6.0, 0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, res, iterations );
	res = QVector3D(2.0/3.0, 1.0/6.0, 0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, res, iterations );
	res = QVector3D(1.0/6.0, 2.0/3.0 ,0);
	intersectSimpleBezierTriangle(ray, intersectionInfo, res, iterations );

	closestdist = intersectionInfo.m_fDistance;
    }

    return fabs(closestdist - k_fMAX) > k_fMIN;
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

bool CTriangle::intersectSimpleBezierTriangle(const CRay &ray, CIntersactionInfo &info, QVector3D &barCoord, unsigned int iterations) const
{

//    return CTriangle::Intersect(ray, info);
    //Planes along ray
    QVector3D N1 = CUtils::Cross(ray.Direction(), QVector3D(-1,-1,-1));
    N1.normalize();
    QVector3D N2 = CUtils::Cross(ray.Direction(), N1);
    N2.normalize();
    double d1 = -CUtils::Dot(N1, ray.StartPoint());
    double d2 = -CUtils::Dot(N2, ray.StartPoint());

//    QMatrix inverseJacobian;
    float invConst;
    QVector3D B, R = QVector3D(0,0,0);
    QVector3D dBu;
    QVector3D dBv;

    for(unsigned int i = 0; i < iterations; i++)
    {
	const double &u(barCoord.x());
	const double &v(barCoord.y());

	//partial U derivative of B
	dBu = 3 * Q30 * u * u
	    + 2 * Q21 * u * v
	    + Q12 * v * v
	    + 2 * Q20 * u
	    + Q11 * v
	    + Q10;

	//Partial V derivative of B
	dBv = 3 * Q03 * v * v
	    + 2 * Q12 * u * v
	    + Q21 * u * u
	    + 2 * Q02 * v
	    + Q11 * u
	    + Q01;

	//Calculating B
	B = Q30 * u*u*u
		+ Q03 * v*v*v
		+ Q21 * u*u*v
		+ Q12 * u*v*v
		+ Q20 * u*u
		+ Q02 * v*v
		+ Q11 * u*v
		+ Q10 * u
		+ Q01 * v
		+ Q00;

	R.setX(CUtils::Dot(N1, B) + d1);
	R.setY(CUtils::Dot(N2, B) + d2);
	if( ( fabs(R.x()) < 1e-6 ) &&
	    ( fabs(R.y()) < 1e-6 ) )
	    break;

	//Inverse Jacobian
	float fN1dotdBu = CUtils::Dot(N1, dBu);
	float fN1dotdBv = CUtils::Dot(N1, dBv);
	float fN2dotdBu = CUtils::Dot(N2, dBu);
	float fN2dotdBv = CUtils::Dot(N2, dBv);

	invConst = 1.0 / ( fN1dotdBu*fN2dotdBv - fN1dotdBv*fN2dotdBu );
	Matrix inverseJacobian;
	inverseJacobian[0][0] = fN2dotdBv*invConst;
	inverseJacobian[0][1] = -fN2dotdBu*invConst;
	inverseJacobian[1][0] = -fN1dotdBu*invConst;
	inverseJacobian[1][1] = fN1dotdBv*invConst;
	inverseJacobian[0][2] = 0;
	inverseJacobian[1][2] = 0;
	inverseJacobian[2][0] = 0;
	inverseJacobian[2][1] = 0;
	inverseJacobian[2][2] = 1;

	//Newton Iteration

	barCoord = barCoord - CUtils::VertexMatrixMultiply(R, inverseJacobian);
    }

    const double &u(barCoord.x());
    const double &v(barCoord.y());
    const double &w(1.0 - u - v);
    barCoord.setZ(w);

    if(u<0.0 || u>1.0 || v<0.0 || v>1.0 || w<0.0 || w>1.0)
	return false;

    //Calculating B
    B = Q30 * u*u*u
	    + Q03 * v*v*v
	    + Q21 * u*u*v
	    + Q12 * u*v*v
	    + Q20 * u*u
	    + Q02 * v*v
	    + Q11 * u*v
	    + Q10 * u
	    + Q01 * v
	    + Q00;

    if( ( fabs(CUtils::Dot(N1, B) + d1) > 1e-2 ) ||
	( fabs(CUtils::Dot(N2, B) + d2) > 1e-2 ) )
	return false;

    // calculate the intersection
    double len = (B-ray.StartPoint()).length();
    if(len > info.m_fDistance)
    {
	return false;
    }

    info.m_vIntersectionPoint = B;
    info.m_fDistance = len;
//    getBezierNormal(barCoord, info);

    //partial U derivative of B
    dBu = 3 * Q30 * u * u
	+ 2 * Q21 * u * v
	+ Q12 * v * v
	+ 2 * Q20 * u
	+ Q11 * v
	+ Q10;

    //Partial V derivative of B
    dBv = 3 * Q03 * v * v
	+ 2 * Q12 * u * v
	+ Q21 * u * u
	+ 2 * Q02 * v
	+ Q11 * u
	+ Q01;

    CUtils::Normal(info.m_vNormal, dBu, dBv);

    return true;
}
