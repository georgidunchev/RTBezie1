#include "beziertriangle.h"
#include <raytracer.h>
#include <main.h>
#include <Utils.h>
#include "cmesh.h"
#include <settings.h>
#include <qmath.h>

CBezierTriangle::CBezierTriangle()
{
}

CBezierTriangle::CBezierTriangle(int nMainTriangleID)
{
    m_aVertecis = GetRaytracer()->GetMesh().GetVertices();
    CPrimitive* Triangle = GetRaytracer()->GetMesh().GetTriangles()[nMainTriangleID];
    const SAdjacencyOfTriangle& AdjTriangles = GetRaytracer()->GetMesh().GetAdjacentTriangles(nMainTriangleID);

    m_aVertIndices.append(Triangle->Vertices()[0]);
    m_aVertIndices.append(Triangle->Vertices()[1]);
    m_aVertIndices.append(Triangle->Vertices()[2]);
    m_aVertIndices.append(AdjTriangles.aAdjacentVertices[0]);
    m_aVertIndices.append(AdjTriangles.aAdjacentVertices[1]);
    m_aVertIndices.append(AdjTriangles.aAdjacentVertices[2]);


    m_bHasBoundingBox = false;
    m_vAB = B() - A();
    m_vAC = C() - A();
    m_vNormal = CUtils::Cross(AB(), AC()).normalized();

    const QVector3D &P110 = m_aVertecis[m_aVertIndices[0]];
    const QVector3D &P011 = m_aVertecis[m_aVertIndices[1]];
    const QVector3D &P101 = m_aVertecis[m_aVertIndices[2]];
    const QVector3D &P020 = m_aVertecis[m_aVertIndices[3]];
    const QVector3D &P002 = m_aVertecis[m_aVertIndices[4]];
    const QVector3D &P200 = m_aVertecis[m_aVertIndices[5]];

    Q20 = P200 + P002 - 2*P101;
    Q02 = P020 + P002 - 2*P011;
    Q11 = 2 * ( P002 + P110 - P011 - P101);
    Q10 = 2 * ( P101 - P002 );
    Q01 = 2 * ( P011 - P002 );
    Q00 = P002;
//    //
//    // create the normals[] array - if needed:
//    if (!hasNormals && autoSmooth ) {
//	    hasNormals = true;
//	    normals.resize(vertices.size(), Vector(0, 0, 0)); // extend the normals[] array, and fill with zeros
//	    for (int i = 0; i < (int) triangles.size(); i++)
//		    for (int j = 0; j < 3; j++) {
//			    triangles[i].n[j] = triangles[i].v[j];
//			    normals[triangles[i].n[j]] += triangles[i].gnormal;
//		    }
//	    for (int i = 1; i < (int) normals.size(); i++)
//		    if (normals[i].lengthSqr() > 1e-9) normals[i].normalize();
//    }
//    return true;

}

bool CBezierTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo) const
{
    float closestdist = k_fMAX;
    intersectionInfo.m_fDistance = k_fMAX;

    bool bezierFast = true;
    if(!bezierFast)
    {
	QVector3D res = QVector3D(1.0/3.0, 1.0/3.0, 0);
	if(intersectSimpleBezierTriangle(ray, intersectionInfo, res, 5 ))
	{
	    closestdist = intersectionInfo.m_fDistance;
	}
	else
	{
	    return false;
	}
    }
    else
    {
	int iterations = 5;
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

bool CBezierTriangle::intersectSimpleBezierTriangle(const CRay &ray, CIntersactionInfo &info, QVector3D &barCoord, unsigned int iterations) const
{

//    return CTriangle::Intersect(ray, info);
    //Planes along ray
    QVector3D N1 = CUtils::Cross(ray.Direction(), QVector3D(-1,-1,-1));
    N1.normalize();
    QVector3D N2 = CUtils::Cross(ray.Direction(), N1);
    N2.normalize();
    double d1 = -CUtils::Dot(N1, ray.StartPoint());
    double d2 = -CUtils::Dot(N2, ray.StartPoint());

    QMatrix inverseJacobian;
    float invConst;
    QVector3D B, R = QVector3D(0,0,0);
    QVector3D dBu;
    QVector3D dBv;

    for(unsigned int i = 0; i < iterations; i++)
    {
	const double &u(barCoord.x());
	const double &v(barCoord.y());

	//partial U derivative of B
	dBu = 2 * Q20 * u + Q11 * v + Q10;

	//Partial V derivative of B
	dBv = 2 * Q02 * v + Q11 * u + Q01;

	//Calculating B
	B = Q20 * u*u + Q02*v*v + Q11*u*v + Q10*u + Q01*v + Q00;

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

    B = Q20 * u*u + Q02*v*v + Q11*u*v + Q10*u + Q01*v + Q00;

    if( ( fabs(CUtils::Dot(N1, B) + d1) > 1e-6 ) ||
	( fabs(CUtils::Dot(N2, B) + d2) > 1e-6 ) )
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

    return true;
}

//void CBezierTriangle::getBezierNormal(const Vector &barCoord, IntersectionInfo &info) const
//{
//    const Vector &normal1(hasNormals ? normals[triangles[0].n[0]] : triangles[0].gnormal);
//    const Vector &normal2(hasNormals ? normals[triangles[1].n[0]] : triangles[1].gnormal);
//    const Vector &normal3(hasNormals ? normals[triangles[2].n[0]] : triangles[2].gnormal);

//    // if we require normal smoothing, we use the interpolation formula, based on the linear system we just solved.
//    info.norm = normal1
//	    + (normal2 - normal1) * barCoord[1]
//	    + (normal3 - normal1) * barCoord[2];
//    // .. and we don't forget to normalize it later
//    info.norm.normalize();
//}


bool CBezierTriangle::Intersect(const QVector3D &vStart, const QVector3D &vEnd) const
{
    return CTriangle::Intersect(vStart, vEnd);
}

