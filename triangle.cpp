#include "triangle.h"
#include <intersactioninfo.h>
#include <ray.h>
#include <QVector3D>
#include <QVector>
#include <qmath.h>
#include <QDebug>
#include <Utils.h>


CTriangle::CTriangle( const QVector<CVertex> &aVertecis, int v1, int v2, int v3)
    :CPrimitive(aVertecis)
{
    m_aVertIndices << v1 << v2 << v3;
    m_vAB = B().GetPos() - A().GetPos();
    m_vAC = C().GetPos() - A().GetPos();
    m_vNormal = CUtils::Cross(AB(), AC()).normalized();
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
