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
{}

CTriangle::CTriangle( const std::vector<CVertex> &aVertecis, int v1, int v2, int v3, int nTriangleId)
	: m_aVertecis(GetRaytracer()->GetMesh().Vertices())
	, m_bHasBoundingBox(false)
	, m_aAdditionalPoints(10, QVector3D(0, 0, 0))
	, m_nTriangleId(nTriangleId)
{
	m_aVertIndices.push_back(v1);
	m_aVertIndices.push_back(v2);
	m_aVertIndices.push_back(v3);
	
	m_aAdditionalPoints[0] = m_aVertecis[m_aVertIndices[0]].GetPos();
	m_aAdditionalPoints[1] = m_aVertecis[m_aVertIndices[1]].GetPos();
	m_aAdditionalPoints[2] = m_aVertecis[m_aVertIndices[2]].GetPos();
	m_vAB = B().GetPos() - A().GetPos();
	m_vAC = C().GetPos() - A().GetPos();
	CUtils::Normal(m_vNormal, AB(), AC());
}

CTriangle::~CTriangle()
{
	m_aSubTriangles.clear();
}

const QVector3D CTriangle::GetPointFromBarycentric(const QVector3D& vCoords)
{
	return GetPointFromBarycentric(vCoords.x(), vCoords.y());
}

const QVector3D CTriangle::GetPointFromBarycentric(const QVector2D& vCoords)
{
	return GetPointFromBarycentric(vCoords.x(), vCoords.y());
}

const QVector3D CTriangle::GetPointFromBarycentric(float u, float v)
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
	o_vNew *= k_fOneThird;
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

void CTriangle::Subdivide()
{
	m_aSubTriangles.resize(CUtils::PowerOf2(k_nNUMBER_OF_SUBDIVISIONS));
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
	if (GetRaytracer()->IsHighQuality())
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

	aPointsToCheck.push_back(QVector3D(intersectionInfo.u,intersectionInfo.v,intersectionInfo.w));
	for (int j = 0; j < 3; j++)
	{
		aPointsToCheck.push_back(SubTriangle.GetVertBar(j));
	}	
		
	float fU = intersectionInfo.u;
	float fV = intersectionInfo.v;
	
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
	return SubTriangle.GetParent().intersectSimpleBezierTriangle(ray, info, barCoord, iterations, bDebug);
}

bool CTriangle::IntersectFast(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
	for (int i = 0; i < 9; i++)
	{
		if (IntersectBezierSubTriangle(ray, intersectionInfo, i, bDebug))
		{
			return true;
		}
	}

	return false;
}

bool CTriangle::IntersectBezierSubTriangle(const CRay &ray, CIntersactionInfo &intersectionInfo, int i_nTriangleId, bool bDebug) const
{
	

	//switch (i_nTriangleId)
	//{
	//case 0:
	//	{
	//		i_vA = GetPoint(3,0); i_vB = GetPoint(2,1); i_vC = GetPoint(2,0); break;
	//	}
	//case 1:
	//	{
	//		i_vA = GetPoint(2,0); i_vB = GetPoint(2,1); i_vC = GetPoint(1,1); break;
	//	}
	//case 2:
	//	{
	//		i_vA = GetPoint(2,1); i_vB = GetPoint(1,2); i_vC = GetPoint(1,1); break;
	//	}
	//case 3:
	//	{
	//		i_vA = GetPoint(1,1); i_vB = GetPoint(1,2); i_vC = GetPoint(0,2); break;
	//	}
	//case 4:
	//	{
	//		i_vA = GetPoint(1,2); i_vB = GetPoint(0,3); i_vC = GetPoint(0,2); break;
	//	}
	//case 5:
	//	{
	//		i_vA = GetPoint(2,0); i_vB = GetPoint(1,1); i_vC = GetPoint(1,0); break;
	//	}
	//case 6:
	//	{
	//		i_vA = GetPoint(1,1); i_vB = GetPoint(0,2); i_vC = GetPoint(0,1); break;
	//	}
	//case 7:
	//	{
	//		i_vA = GetPoint(1,0); i_vB = GetPoint(1,1); i_vC = GetPoint(0,1); break;
	//	}
	//case 8:
	//	{
	//		i_vA = GetPoint(1,0); i_vB = GetPoint(0,1); i_vC = GetPoint(0,0); break;

	//	}
	//}

	int u1,u2,u3;
	int v1,v2,v3;
	int w1,w2,w3;

	switch (i_nTriangleId)
	{
	case 0:
		{
			u1 = 3; v1 = 0; w1 = 3 - u1 - v1; u2 = 2; v2 = 1; w2 = 3 - u2 - v2; u3 = 2; v3 = 0; w3 = 3 - u3 - v3; break;
		}
	case 1:
		{
			u1 = 2; v1 = 0; w1 = 3 - u1 - v1; u2 = 2; v2 = 1; w2 = 3 - u2 - v2; u3 = 1; v3 = 1; w3 = 3 - u3 - v3; break;
		}
	case 2:
		{
			u1 = 2; v1 = 1; w1 = 3 - u1 - v1; u2 = 1; v2 = 2; w2 = 3 - u2 - v2; u3 = 1; v3 = 1; w3 = 3 - u3 - v3; break;
		}
	case 3:
		{
			u1 = 1; v1 = 1; w1 = 3 - u1 - v1; u2 = 1; v2 = 2; w2 = 3 - u2 - v2; u3 = 0; v3 = 2; w3 = 3 - u3 - v3; break;
		}
	case 4:
		{
			u1 = 1; v1 = 2; w1 = 3 - u1 - v1; u2 = 0; v2 = 3; w2 = 3 - u2 - v2; u3 = 0; v3 = 2; w3 = 3 - u3 - v3; break;
		}
	case 5:
		{
			u1 = 2; v1 = 0; w1 = 3 - u1 - v1; u2 = 1; v2 = 1; w2 = 3 - u2 - v2; u3 = 1; v3 = 0; w3 = 3 - u3 - v3; break;
		}
	case 6:
		{
			u1 = 1; v1 = 1; w1 = 3 - u1 - v1; u2 = 0; v2 = 2; w2 = 3 - u2 - v2; u3 = 0; v3 = 1; w3 = 3 - u3 - v3; break;
		}
	case 7:
		{
			u1 = 1; v1 = 0; w1 = 3 - u1 - v1; u2 = 1; v2 = 1; w2 = 3 - u2 - v2; u3 = 0; v3 = 1; w3 = 3 - u3 - v3; break;
		}
	case 8:
		{
			u1 = 1; v1 = 0; w1 = 3 - u1 - v1; u2 = 0; v2 = 1; w2 = 3 - u2 - v2; u3 = 0; v3 = 0; w3 = 3 - u3 - v3; break;

		}
	}

	const QVector3D i_vA = GetPoint(u1,v1);
	const QVector3D i_vB = GetPoint(u2,v2);
	const QVector3D i_vC = GetPoint(u3,v3);
	
	QVector3D vPA(u1,v1,w1);
	QVector3D vPB(u2,v2,w2);
	QVector3D vPC(u3,v3,w3);
	vPA *= k_fOneThird;
	vPB *= k_fOneThird;
	vPC *= k_fOneThird;

	if ( CUtils::IntersectTriangle(ray, intersectionInfo, i_vA, i_vB, i_vC, vPA, vPB, vPC) )
	{																	   
		if (bDebug)
		{
			char str[100];
			sprintf( str, "Approximation u: %4.2f v: %4.2f w: %4.2f", intersectionInfo.u, intersectionInfo.v, intersectionInfo.w);
			qDebug() << str;
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool CTriangle::IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, std::vector<QVector3D>* aPointsToCheck, bool bDebug) const
{
	return IntersectSubdevidedTriangles(ray, intersectionInfo, m_aSubTriangles, aPointsToCheck, bDebug);
}

bool CTriangle::IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, std::vector<QVector3D>* aPointsToCheck, bool bDebug)
{
	int nSize = aSubTriangles.size();
	float fModifier = 8.0f / static_cast<float>(nSize);

	for (int i = 0; i < nSize; i++)
	{
		if( aSubTriangles[i]->Intersect(ray, intersectionInfo,bDebug) )
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

				intersectionInfo.color = CColor(fR, fG, fB);
			}
			
			intersectionInfo.pSubTriangle = aSubTriangles[i];
		
			intersectionInfo.m_nSubTriangleId = nId;

			return true;
		}
	}
	return false;
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

bool CTriangle::intersectSimpleBezierTriangle(const CRay &ray, CIntersactionInfo &info, QVector3D &barCoord, unsigned int iterations, bool bDebug) const
{
	//    return CTriangle::Intersect(ray, info);
	//Planes along ray
	QVector3D N1 = CUtils::Cross(ray.Direction(), QVector3D(-1,-1,-1));
	N1.normalize();
	QVector3D N2 = CUtils::Cross(ray.Direction(), N1);
	N2.normalize();
	double d1 = -CUtils::Dot(N1, ray.StartPoint());
	double d2 = -CUtils::Dot(N2, ray.StartPoint());

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

	const double &u(barCoord.x());
	const double &v(barCoord.y());
	const double w(1.0 - u - v);
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
	double len = (B - ray.StartPoint()).length();
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

	CUtils::Normal(info.m_vNormal, dBu, dBv);

	return true;
}

