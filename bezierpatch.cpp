#include "bezierpatch.h"
#include <intersactioninfo.h>
#include <ray.h>
#include <Utils.h>
#include <qmath.h>
#include <QDebug>
#include "triangle.h"
#include "SubTriangle.h"

CBezierPatch::CBezierPatch(CTriangle& pParent)
    : m_Parent_Triangle(pParent)
    , m_pParent_SubTriangle(NULL)
    , m_aAdditionalPoints(10, QVector3D(0, 0, 0))
{
}

CBezierPatch::CBezierPatch(CSubTriangle *pParent)
    : m_Parent_Triangle(pParent->GetParent())
    , m_pParent_SubTriangle(pParent)
    , m_aAdditionalPoints(10, QVector3D(0, 0, 0))
{
}

const QVector3D CBezierPatch::GetPointFromBarycentric(const QVector3D& vCoords) const
{
    return GetPointFromBarycentric(vCoords.x(), vCoords.y());
}

const QVector3D CBezierPatch::GetPointFromBarycentric(const QVector2D& vCoords) const
{
    return GetPointFromBarycentric(vCoords.x(), vCoords.y());
}

const QVector3D CBezierPatch::GetPointFromBarycentric(float u, float v) const
{
    QVector3D B = Q30 * u*u*u
	    + Q03 * v*v*v
	    + Q21 * u*u*v
	    + Q12 * u*v*v
	    + Q20 * u*u
	    + Q02 * v*v
	    + Q11 * u*v
	    + Q10 * u
	    + Q01 * v
	    + Q00;
    return B;
}

void CBezierPatch::BuildBezierPoints()
{
    m_aAdditionalPoints[0] = m_Parent_Triangle.A().GetPos();
    m_aAdditionalPoints[1] = m_Parent_Triangle.B().GetPos();
    m_aAdditionalPoints[2] = m_Parent_Triangle.C().GetPos();

    std::vector<CVertexInfo> aVertices;
    // Push the needed information for generating the additional points
    aVertices.push_back(CVertexInfo(Point(3,0),
				    Point(0,3), Point(2,1),
				    Point(0,0), Point(2,0),
				    m_Parent_Triangle.A().Normal_Get()));

    aVertices.push_back(CVertexInfo(Point(0,3),
				    Point(0,0), Point(0,2),
				    Point(3,0), Point(1,2),
				    m_Parent_Triangle.B().Normal_Get()));

    aVertices.push_back(CVertexInfo(Point(0,0),
				    Point(3,0), Point(1,0),
				    Point(0,3), Point(0,1),
				    m_Parent_Triangle.C().Normal_Get()));

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

    BuildBezierPoints_Internal();
}

void CBezierPatch::BuildBezierPoints_InitialSub()
{
    for (uint i = 0; i < 10; ++i)
    {
	Point(i) = m_pParent_SubTriangle->GetParent().GetBezierPatch().Point(i);
    }
    BuildBezierPoints_Internal();
}

void CBezierPatch::BuildBezierPoints_Sub(int nStartOfLongest, bool bFirst)
{
    int a[3], b[3];
    switch (nStartOfLongest)
    {
    case 0:
    {
	a[0] = a[1] = a[2] = 3;
	b[0] = b[1] = b[2] = 0;
	break;
    }
    case 1:
    {
	a[0] = a[1] = a[2] = 0;
	b[0] = b[1] = b[2] = 3;
	break;
    }
    case 2:
    {
	a[0] = a[1] = a[2] = 0;
	b[0] = b[1] = b[2] = 0;
	break;
    }
    }

    int pos1 = (2 + nStartOfLongest)%3;

    CUtils::GetNextPoint(a[1], b[1], nStartOfLongest, -3);
    CUtils::GetNextPoint(a[2], b[2], pos1, 3);

    CBezierPatch& ParentPatch = *m_pParent_SubTriangle->GetParentSubTriangle()->GetBezierPatch();

    //003
    Point(a[2], b[2]) = ParentPatch.GetPoint(a[2],b[2]);
//    qDebug() << a[2] << b[2] << (bFirst?"Left":"Right");

    std::vector<int> c;
    std::vector<int> d;
    QVector3D t[4];
    //102 &012
    c.assign(2, a[2]);
    d.assign(2, b[2]);
    CUtils::GetNextPoint(c[0], d[0], (2 + nStartOfLongest)%3, -1);
    CUtils::GetNextPoint(c[1], d[1], (1 + nStartOfLongest)%3, 1);

    if (!bFirst)
    {
	Reverse(c, d);
    }

//    qDebug() << c[0] << d[0] << "," << c[1] << d[1];

    t[0] = ParentPatch.GetPoint(c[0],d[0]);
    t[1] = ParentPatch.GetPoint(c[1],d[1]);
    Point(c[0], d[0]) = t[0];
    Point(c[1], d[1]) = (t[0] + t[1]) * 0.5f;

    //201 & 111 & 021
    c.clear();
    d.clear();
    c.assign(3, a[2]);
    d.assign(3, b[2]);
    CUtils::GetNextPoint(c[0], d[0], (2 + nStartOfLongest)%3, -2);
    c[1] = 1; d[1] = 1;
    CUtils::GetNextPoint(c[2], d[2], (1 + nStartOfLongest)%3, 2);

    if (!bFirst)
    {
	Reverse(c, d);
    }

//    qDebug() << c[0] << d[0] << "," << c[1] << d[1] << "," << c[2] << d[2];

    t[0] = ParentPatch.GetPoint(c[0],d[0]);
    t[1] = ParentPatch.GetPoint(c[1],d[1]);
    t[2] = ParentPatch.GetPoint(c[2],d[2]);
    Point(c[0], d[0]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    t[1] = (t[1]+t[2]) * 0.5f;
    Point(c[1], d[1]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    Point(c[2], d[2]) = t[0];

    //030 & 210 & 120 & 030
    c.clear();
    d.clear();
    c.assign(4, a[0]);
    d.assign(4, b[0]);
    CUtils::GetNextPoint(c[1], d[1], nStartOfLongest, -1);
    CUtils::GetNextPoint(c[2], d[2], nStartOfLongest, -2);
    c[3] = a[1]; d[3]= b[1];

    if (!bFirst)
    {
	Reverse(c, d);
    }

//    qDebug() << c[0] << d[0] << "," << c[1] << d[1] << "," << c[2] << d[2] << "," << c[3] << d[3];

    t[0] = ParentPatch.GetPoint(c[0],d[0]);
    t[1] = ParentPatch.GetPoint(c[1],d[1]);
    t[2] = ParentPatch.GetPoint(c[2],d[2]);
    t[3] = ParentPatch.GetPoint(c[3],d[3]);

    Point(c[0], d[0]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    t[1] = (t[1]+t[2]) * 0.5f;
    t[2] = (t[2] + ParentPatch.GetPoint(c[3],d[3])) * 0.5f;
    Point(c[1], d[1]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    t[1] = (t[1]+t[2]) * 0.5f;
    Point(c[2], d[2]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    Point(c[3], d[3]) = t[0];
    BuildBezierPoints_Internal();
}

void CBezierPatch::BuildBezierPoints_Internal()
{
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

void CBezierPatch::BuildBezierPoint(CVertexInfo &o_Info)
{
    BuildBezierPoint(*o_Info.vNew1, *o_Info.vMain, *o_Info.vEnd1, *o_Info.vNormal);
    BuildBezierPoint(*o_Info.vNew2, *o_Info.vMain, *o_Info.vEnd2, *o_Info.vNormal);
}

void CBezierPatch::BuildBezierPoint(QVector3D &o_vNew,
				 const QVector3D &i_vMain,
				 const QVector3D &i_vEnd,
				 const QVector3D &i_vNormal)
{
    o_vNew = i_vEnd - i_vMain;
    o_vNew *= k_fOneThird;
    o_vNew = CUtils::ProjectionOfVectorInPlane(o_vNew, i_vNormal);
    o_vNew += i_vMain;
}

void CBezierPatch::Reverse(std::vector<int> &io_c, std::vector<int> &io_d)
{
    for (uint i = 0, n = io_c.size(); i < n/2; ++i)
    {
	const int j = n-1-i;
	std::swap(io_c[i], io_c[j]);
	std::swap(io_d[i], io_d[j]);
    }
}

int CBezierPatch::GetIndex(int a, int b) const
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

QVector3D& CBezierPatch::Point(int a, int b)
{
    return m_aAdditionalPoints[GetIndex(a,b)];
}

QVector3D &CBezierPatch::Point(int idx)
{
    return m_aAdditionalPoints[idx];
}

const QVector3D &CBezierPatch::GetPoint(int a, int b) const
{
    return m_aAdditionalPoints[GetIndex(a,b)];
}

const QVector3D &CBezierPatch::GetPoint(QVector2D bar) const
{
    return m_aAdditionalPoints[GetIndex(bar.x(),bar.y())];
}

bool CBezierPatch::IntersectHighQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
//    return IntersectLowQuality(ray, intersectionInfo, bDebug);

//    return false;
    std::vector<QVector3D> aPointsToCheck;
//    CSubTriangle& SubTriangle = * intersectionInfo.pSubTriangle;

//    aPointsToCheck.push_back(QVector3D(intersectionInfo.u,intersectionInfo.v,intersectionInfo.w));
//    for (int j = 0; j < 3; j++)
//    {
//	aPointsToCheck.push_back(SubTriangle.GetVertBar(j));
//    }

    aPointsToCheck.resize(10);
    int u = 0, v = 0;
    uint k = 0;
    for (uint i = 0; i < 4; ++i)
    {
	if (i > 0)
	{
	    CUtils::GetNextPoint(u, v, 2, -1);
	}

	aPointsToCheck[k] = QVector3D(u, v, 3 - u - v);
	aPointsToCheck[k] /= 3.0f;
	k++;

	int u1 = u, v1 = v;
	for (uint j = 0; j < i; ++j)
	{
	    CUtils::GetNextPoint(u1, v1, 0, -1);
	    aPointsToCheck[k] = QVector3D(u1, v1, 3 - u1 - v1);
	    aPointsToCheck[k] /= 3.0f;
	    k++;
	}
    }

//    for (uint i = 0, n = m_aAdditionalPoints.size(); i < n; ++i)
//    {
//	aPointsToCheck.push_back(GetPointFromBarycentric(m_aAdditionalPoints[i]));
//    }

//    float fU = intersectionInfo.u;
//    float fV = intersectionInfo.v;

    float closestdist = k_fMAX;
    intersectionInfo.m_fDistance = k_fMAX;

    bool bezierFast = false;
    if (!bezierFast)
    {
	for (int i = 0, n = aPointsToCheck.size(); i < n; ++i)
	{
	    if (intersect(ray, intersectionInfo, aPointsToCheck[i], 15, bDebug))
	    {
		closestdist = intersectionInfo.m_fDistance;
		return true;
	    }
	}
	return false;
    }
//    else
//    {
//	int iterations = 5;
//	QVector3D res = QVector3D(1.0/3.0, 1.0/3.0, 0);
//	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
//	res = QVector3D(1.0/6.0, 1.0/6.0, 0);
//	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
//	res = QVector3D(2.0/3.0, 1.0/6.0, 0);
//	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
//	res = QVector3D(1.0/6.0, 2.0/3.0 ,0);
//	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );

//	closestdist = intersectionInfo.m_fDistance;
//    }

    return fabs(closestdist - k_fMAX) > k_fMIN;
}

bool CBezierPatch::IntersectLowQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
    std::vector<QVector2D> aFirst;
    aFirst.push_back(QVector2D(3,0));
    aFirst.push_back(QVector2D(2,1));
    aFirst.push_back(QVector2D(1,2));
    aFirst.push_back(QVector2D(2,0));
    aFirst.push_back(QVector2D(1,1));
    aFirst.push_back(QVector2D(1,0));

    for(int i = 0, m = aFirst.size(); i < m; ++i)
    {
	int u = aFirst[i].x(), v = aFirst[i].y();
	QVector3D vA = GetPoint(aFirst[i]);
	CUtils::GetNextPoint(u, v, 0, -1);
	QVector3D vB = GetPoint(u, v);
	CUtils::GetNextPoint(u, v, 1, -1);
	QVector3D vC = GetPoint(u, v);

	if (CUtils::IntersectTriangle(ray, intersectionInfo, vA, vB, vC))
	{
	    return true;
	}
    }
    return false;
}

bool CBezierPatch::intersect(const CRay &ray, CIntersactionInfo &info, QVector3D barCoord, unsigned int iterations, bool bDebug) const
{
    //    return CTriangle::Intersect(ray, info);
    //Planes along ray
    QVector3D N1 = CUtils::Cross(ray.Direction(), QVector3D(-1,-1,-1));
    N1.normalize();
    QVector3D N2 = CUtils::Cross(ray.Direction(), N1);
    N2.normalize();
    float d1 = -CUtils::Dot(N1, ray.StartPoint());
    float d2 = -CUtils::Dot(N2, ray.StartPoint());

    float fSMall = 1e-3;

    //    QMatrix inverseJacobian;
    float invConst;
    QVector3D B, R = QVector3D(0,0,0);
    QVector3D dBu;
    QVector3D dBv;

    for (unsigned int i = 0; i < iterations; i++)
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
	if ( ( fabs(R.x()) < fSMall ) &&
	     ( fabs(R.y()) < fSMall ) )
	{
	    break;
	}

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

	if (bDebug)
	{
	    char str[100];
	    sprintf( str, "Iteration %d u: %4.2f v: %4.2f w: %4.2f", i, u, v, 1.0-u-v);
	    qDebug() << str;
	}
    }

    const float &u(barCoord.x());
    const float &v(barCoord.y());
    const float w(1.0 - u - v);
    barCoord.setZ(w);

    if (u<0.0 || u>1.0 || v<0.0 || v>1.0 || w<0.0 || w>1.0)
    {
	return false;
    }

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

    if ( ( fabs(CUtils::Dot(N1, B) + d1) > fSMall ) ||
	 ( fabs(CUtils::Dot(N2, B) + d2) > fSMall ) )
    {
	return false;
    }

    // calculate the intersection
    float len = (B - ray.StartPoint()).length();
    if (len > info.m_fDistance)
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

    //CUtils::Normal(info.m_vNormal, dBu, dBv);

    info.m_vBarCoordsLocal.setX(u);
    info.m_vBarCoordsLocal.setY(v);
    info.m_vBarCoordsLocal.setZ(w);

    info.m_vBarCoordsGlobal = m_pParent_SubTriangle->GetParentBar(info.m_vBarCoordsLocal);

    const QVector3D vNormalA = info.m_vBarCoordsGlobal.x() * m_Parent_Triangle.A().Normal_Get();
    const QVector3D vNormalB = info.m_vBarCoordsGlobal.y() * m_Parent_Triangle.B().Normal_Get();
    const QVector3D vNormalC = info.m_vBarCoordsGlobal.z() * m_Parent_Triangle.C().Normal_Get();
    info.m_vNormal = vNormalA + vNormalB + vNormalC;
    info.m_vNormal.normalize();

    if (bDebug)
    {
	qDebug()<< "Patch Hit!";
    }

    return true;
}

