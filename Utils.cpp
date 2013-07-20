#include <Utils.h>
#include <ray.h>
#include <intersactioninfo.h>
#include <qmath.h>

CUtils::CUtils(QObject *parent)
	: QObject(parent)
{
}

float CUtils::GetDimension(const QVector3D& pVector, EDimiensions eDimension)
{
    switch(eDimension)
    {
    case e_Dimension_X:
    {
	return pVector.x();
    }
    case e_Dimension_Y:
    {
	return pVector.y();
    }
    case e_Dimension_Z:
    {
	return pVector.z();
    }
    }
    return 0.f;
}

void CUtils::SetDimension(QVector3D &pVector, EDimiensions eDimension, const float fValue)
{
    switch(eDimension)
    {
    case e_Dimension_X:
    {
	pVector.setX(fValue);
	break;
    }
    case e_Dimension_Y:
    {
	pVector.setY(fValue);
	break;
    }
    case e_Dimension_Z:
    {
	pVector.setZ(fValue);
	break;
    }
    }
}

void CUtils::SafeDel(void *pointer)
{
    if (pointer)
    {
	delete pointer;
    }
}

QVector3D CUtils::GetPointAtDistance(const CRay &ray, float fDistance)
{
    QVector3D vNewPos = ray.Direction();
    vNewPos *= fDistance;
    vNewPos += ray.StartPoint();
    return vNewPos;
}

float  CUtils::Dot(QVector3D vec1, QVector3D vec2)
{
    return QVector3D::dotProduct(vec1, vec2);
}

QVector3D  CUtils::Cross(QVector3D vec1, QVector3D vec2)
{
    return QVector3D::crossProduct(vec1, vec2);
}

float CUtils::Triple(QVector3D vec1, QVector3D vec2, QVector3D vec3)
{
    return Dot(vec1, Cross(vec2, vec3) );
}

QVector3D CUtils::VertexMatrixMultiply(const QVector3D& v, const Matrix& m)
{
	return QVector3D(
		v.x() * m[0][0] + v.y() * m[1][0] + v.z() * m[2][0],
		v.x() * m[0][1] + v.y() * m[1][1] + v.z() * m[2][1],
		v.x() * m[0][2] + v.y() * m[1][2] + v.z() * m[2][2] );
}

float CUtils::Cos(QVector3D vec1, QVector3D vec2)
{
    vec1.normalize();
    vec2.normalize();
    return Dot(vec1, vec2);
}

QVector3D CUtils::ProjectionOfVectorInPlane(QVector3D vVector, QVector3D vNormalOfPlane)
{
    QVector3D vHelper = Cross(vVector, vNormalOfPlane);
    QVector3D vProjection = Cross(vNormalOfPlane, vHelper);
    vProjection.normalize();
    double fLen = Dot(vVector, vProjection);
    return vProjection*fLen;
}

void CUtils::Normal(QVector3D &o_vNormal, const QVector3D &i_vAB, const QVector3D &i_vAC)
{
    o_vNormal = CUtils::Cross(i_vAB, i_vAC).normalized();
}

void CUtils::TriangleCentre(QVector3D &o_vCentre,
			    const QVector3D &i_vA,
			    const QVector3D &i_vB,
			    const QVector3D &i_vC)
{
    o_vCentre = i_vA + i_vB + i_vC;
    o_vCentre /= 3;
}

bool CUtils::IntersectTriangle(const CRay &i_Ray,
			       CIntersactionInfo &io_IntersectionInfo,
			       const QVector3D &i_vA,
			       const QVector3D &i_vB,
			       const QVector3D &i_vC)
{

    //intersect stuff
    double lambda2;
    double lambda3;
    double closestdist = io_IntersectionInfo.m_fDistance;

    const QVector3D a = i_vB - i_vA;
    const QVector3D b = i_vC - i_vA;

    QVector3D c = - (i_Ray.Direction());
    QVector3D h = i_Ray.StartPoint() - i_vA;
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
    const float Dcr = CUtils::Triple(a, b, c);
    // check for zero; if it is zero, then the triangle and the ray are parallel
    if (fabs(Dcr) < k_fSMALL)
	{
		return false;
	}
    // find the reciprocal of the determinant. We would use this quantity later in order
    // to multiply by rDcr instead of divide by Dcr (division is much slower)
    double rDcr = 1.0 / Dcr;
    // calculate `gamma' by substituting the right part of the equation in the third column of the matrix,
    // getting the determinant, and dividing by Dcr)
    const double gamma = CUtils::Triple(a, b, h) * rDcr;
    // Is the intersection point behind us?  Is the intersection point worse than what we currently have?
    if (gamma <= 0 || gamma > closestdist)
	{
		return false;
	}
    lambda2 = CUtils::Triple(h, b, c) * rDcr;
    // Check if it is in range (barycentric coordinates)
    if (lambda2 < 0 || lambda2 > 1)
	{
		return false;
	}
    lambda3 = CUtils::Triple(a, h, c) * rDcr;

    // Calculate lambda3 and check if it is in range as well
    if (lambda3 < 0 || lambda3 > 1)
	{
		return false;
	}
    if (lambda2 + lambda3 > 1)
	{
		return false;
	}

    closestdist = gamma;
    io_IntersectionInfo.m_fDistance = closestdist;
    Normal(io_IntersectionInfo.m_vNormal, a, c);
    io_IntersectionInfo.m_vIntersectionPoint = CUtils::GetPointAtDistance(i_Ray, closestdist);
	io_IntersectionInfo.u = 1.0f - lambda2 - lambda3;
	io_IntersectionInfo.v = lambda3;
	io_IntersectionInfo.w = lambda2;
    return true;
}

int CUtils::PowerOf2(const int nPow)
{
	if (nPow > 1)
	{
		return 2 << nPow -1;
	}
	else if (nPow < 1)
	{
		return 2 >> 1 - nPow;
	} 
	else
	{
		return 2;
	}
}

void CUtils::AddDebugString(const char* str)
{
	strDebugOut.append( QString(str) );
	strDebugOut.append( "\n");
	//emit DebugOutChanged(strDebugOut);
}