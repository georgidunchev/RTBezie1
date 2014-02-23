#include "Utils.h"
#include "ray.h"
#include "intersactioninfo.h"
#include "qmath.h"

CUtils::CUtils()
{
}


void CUtils::SafeDel(void *pointer)
{
    if (pointer)
    {
	delete pointer;
    }
}




void CUtils::TriangleCentre(CVector3DF &o_vCentre,
			    const CVector3DF &i_vA,
			    const CVector3DF &i_vB,
			    const CVector3DF &i_vC)
{
    o_vCentre = i_vA + i_vB + i_vC;
    o_vCentre /= 3;
}

bool CUtils::IntersectTriangle(const CRay &i_Ray,
			       CIntersactionInfo &io_IntersectionInfo,
			       const CVector3DF &i_vA,
			       const CVector3DF &i_vB,
			       const CVector3DF &i_vC)
{

    //intersect stuff
    double lambda2;
    double lambda3;
    double closestdist = io_IntersectionInfo.m_fDistance;

    const CVector3DF a = i_vB - i_vA;
    const CVector3DF b = i_vC - i_vA;

    CVector3DF c = - (i_Ray.Direction());
    CVector3DF h = i_Ray.StartPoint() - i_vA;
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
    const float Dcr = CVector3DF::Triple(a, b, c);
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
    const double gamma = CVector3DF::Triple(a, b, h) * rDcr;
    // Is the intersection point behind us?  Is the intersection point worse than what we currently have?
    if (gamma <= 0 || gamma > closestdist)
    {
	return false;
    }
    lambda2 = CVector3DF::Triple(h, b, c) * rDcr;
    // Check if it is in range (barycentric coordinates)
    if (lambda2 < 0 || lambda2 > 1)
    {
	return false;
    }
    lambda3 = CVector3DF::Triple(a, h, c) * rDcr;

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
    
    io_IntersectionInfo.m_vIntersectionPoint = i_Ray.GetPointAtDistance(closestdist);

    io_IntersectionInfo.m_vBarCoordsLocal.SetX(1.0f - lambda2 - lambda3);
    io_IntersectionInfo.m_vBarCoordsLocal.SetY(lambda2);
    io_IntersectionInfo.m_vBarCoordsLocal.SetZ(lambda3);

    //Normal to the surface
    io_IntersectionInfo.m_vNormal = CVector3DF::Normal(a, b);

    return true;
}


bool CUtils::IntersectTriangle(const CRay &i_Ray,
			       CIntersactionInfo &io_IntersectionInfo,
			       const CVector3DF& i_vA,
			       const CVector3DF& i_vB,
			       const CVector3DF& i_vC,
			       const CVector3DF& i_vABar,
			       const CVector3DF& i_vBBar,
			       const CVector3DF& i_vCBar)
{
    if (!IntersectTriangle(i_Ray, io_IntersectionInfo, i_vA, i_vB, i_vC))
    {
	return false;
    }

    const CVector3DF vPA = i_vABar * io_IntersectionInfo.m_vBarCoordsLocal.X();
    const CVector3DF vPB = i_vBBar * io_IntersectionInfo.m_vBarCoordsLocal.Y();
    const CVector3DF vPC = i_vCBar * io_IntersectionInfo.m_vBarCoordsLocal.Z();

    io_IntersectionInfo.m_vBarCoordsGlobal = vPA + vPB + vPC;

    return true;
}


int CUtils::PowerOf2(const int nPow)
{
    if (nPow > 1)
    {
	return 2 << (nPow -1);
    }
    else if (nPow < 1)
    {
	return 2 >> (1 - nPow);
    }
    else
    {
	return 2;
    }
}

CVector3DF CUtils::Reflect(const CVector3DF &toBeReflected, const CVector3DF &normal)
{
    return toBeReflected + 2 * (normal.Dot(-toBeReflected)) * normal;
}

void CUtils::GetNextPoint(int &io_a, int &io_b, int nPos, int nMod)
{
    int point[3] = {io_a, io_b, 3 - io_a - io_b};
    int nPos2 = (nPos+1)%3;
    int nPos3 = (nPos+2)%3;
    point[nPos] = point[nPos] + nMod;
    point[nPos2] = point[nPos2] - nMod ;
    point[nPos3] = 3 - point[nPos] - point[nPos2];

    io_a = point[0];
    io_b = point[1];
}
