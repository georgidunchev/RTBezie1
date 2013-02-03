#include "triangle.h"
#include <intersactioninfo.h>
#include <ray.h>
#include <QVector3D>
#include <QVector>
#include <qmath.h>
#include <QDebug>

static qreal Dot(QVector3D vec1, QVector3D vec2)
{
    return QVector3D::dotProduct(vec1, vec2);
}

static QVector3D Cross(QVector3D vec1, QVector3D vec2)
{
    return QVector3D::crossProduct(vec1, vec2);
}

static qreal Triple(QVector3D vec1, QVector3D vec2, QVector3D vec3)
{
    return Dot(vec1, Cross(vec2, vec3) );
}

CTriangle::CTriangle( const QVector<QVector3D> & aVertecis, int v1, int v2, int v3)
    :m_aVertecis(aVertecis)
    , m_bHasBoundingBox(false)
{
    m_aVertIndices << v1 << v2 << v3;
    m_vAB = B() - A();
    m_vAC = C() - A();
    m_vNormal = Cross(AB(), AC());
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
    QVector3D h = ray.StartPoint() - A();
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
    double Dcr = Triple(AB(), AC(), c);
    // check for zero; if it is zero, then the triangle and the ray are parallel
    if (fabs(Dcr) < 1e-9)
	return false;
    // find the reciprocal of the determinant. We would use this quantity later in order
    // to multiply by rDcr instead of divide by Dcr (division is much slower)
    double rDcr = 1.0 / Dcr;
    // calculate `gamma' by substituting the right part of the equation in the third column of the matrix,
    // getting the determinant, and dividing by Dcr)
    double gamma = Triple(AB(), AC(), h) * rDcr;
    // Is the intersection point behind us?  Is the intersection point worse than what we currently have?
    if (gamma <= 0 || gamma > closestdist)
	return false;
    lambda2 = Triple(h, AC(), c) * rDcr;
    // Check if it is in range (barycentric coordinates)
    if (lambda2 < 0 || lambda2 > 1)
	return false;
    lambda3 = Triple(AB(), h, c) * rDcr;

    // Calculate lambda3 and check if it is in range as well
    if (lambda3 < 0 || lambda3 > 1)
	return false;
    if (lambda2 + lambda3 > 1)
	return false;

    closestdist = gamma;
    return true;

    intersectionInfo.m_fDistance = 1.f;
    return true;
}

const QVector<int> &CTriangle::Vertices() const
{
    return m_aVertIndices;
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

const QVector3D &CTriangle::A() const
{
    return m_aVertecis[m_aVertIndices[0]];
}

const QVector3D &CTriangle::B() const
{
    return m_aVertecis[m_aVertIndices[1]];
}

const QVector3D &CTriangle::C() const
{
    return m_aVertecis[m_aVertIndices[2]];
}

void CTriangle::MakeBoundingBox()
{
    m_vMinVertex = A();
    m_vMaxVertex = A();
    for(int i = 1; i < Vertices().size(); ++i)
    {
	if(m_vMinVertex.x() > m_aVertecis[m_aVertIndices[i]].x())
	{
	    m_vMinVertex.setX( m_aVertecis[m_aVertIndices[i]].x());
	}
	if(m_vMinVertex.x() > m_aVertecis[m_aVertIndices[i]].x())
	{
	    m_vMinVertex.setX( m_aVertecis[m_aVertIndices[i]].x());
	}
	if(m_vMinVertex.x() > m_aVertecis[m_aVertIndices[i]].x())
	{
	    m_vMinVertex.setX( m_aVertecis[m_aVertIndices[i]].x());
	}

	if(m_vMaxVertex.x() < m_aVertecis[m_aVertIndices[i]].x())
	{
	    m_vMaxVertex.setX( m_aVertecis[m_aVertIndices[i]].x());
	}
	if(m_vMaxVertex.x() < m_aVertecis[m_aVertIndices[i]].x())
	{
	    m_vMaxVertex.setX( m_aVertecis[m_aVertIndices[i]].x());
	}
	if(m_vMaxVertex.x() < m_aVertecis[m_aVertIndices[i]].x())
	{
	    m_vMaxVertex.setX( m_aVertecis[m_aVertIndices[i]].x());
	}
    }
    m_bHasBoundingBox = true;
}

