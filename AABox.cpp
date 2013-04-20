#include "AABox.h"
#include <ray.h>
#include <QVector3D>
#include <QVector>
#include <qmath.h>
#include <QDebug>
#include <Utils.h>
#include <settings.h>
#include <vertex.h>

CAABox::CAABox()
    : m_vMaxVertex(QVector3D(k_fMIN,k_fMIN,k_fMIN))
    , m_vMinVertex(QVector3D(k_fMAX,k_fMAX,k_fMAX))
{
}

CAABox::CAABox(const QVector3D &vMinVertex, const QVector3D &vMaxVertex)
{
    m_vMinVertex = vMinVertex;
    m_vMaxVertex = vMaxVertex;
}

void CAABox::Set(const QVector3D &vMinVertex, const QVector3D &vMaxVertex)
{
    m_vMinVertex = vMinVertex;
    m_vMaxVertex = vMaxVertex;
}

void CAABox::Reset()
{
    m_vMaxVertex = QVector3D(k_fMIN,k_fMIN,k_fMIN);
    m_vMinVertex = QVector3D(k_fMAX,k_fMAX,k_fMAX);
}

/// Checks if a point is inside the bounding box (borders-inclusive)
bool CAABox::IsInside(const QVector3D& vPoint) const
{
    bool bX = m_vMinVertex.x() - k_fSMALL <= vPoint.x() && vPoint.x() <= m_vMaxVertex.x() + k_fSMALL;
    bool bY = m_vMinVertex.y() - k_fSMALL  <= vPoint.y() && vPoint.y() <= m_vMaxVertex.y() + k_fSMALL;
    bool bZ = m_vMinVertex.z() - k_fSMALL  <= vPoint.z() && vPoint.z() <= m_vMaxVertex.z() + k_fSMALL;
    return (bX && bY && bZ);
}
/// Checks if a point is inside the bounding box (borders-inclusive)
bool CAABox::IsInside(const CVertex& vPoint) const
{
    return IsInside(vPoint.GetPos());
}

void CAABox::AddPoint(const CVertex& vPoint)
{
    AddPoint(vPoint.GetPos());
}

void CAABox::AddPoint(const QVector3D& vPoint)
{
    for(EDimiensions i = e_Dimension_X; i < e_Dimension_MAX; i = (EDimiensions)((int)i + 1) )
    {
	const float fMinVDir( CUtils::GetDimension(m_vMinVertex, i) );
	const float fMaxVDir( CUtils::GetDimension(m_vMaxVertex, i) );
	const float fPointDir( CUtils::GetDimension(vPoint, i) );

	if(fMinVDir > fPointDir)
	{
	    CUtils::SetDimension(m_vMinVertex, i, fPointDir);
	}

	if(fMaxVDir < fPointDir)
	{
	    CUtils::SetDimension(m_vMaxVertex, i, fPointDir);
	}
    }
}


bool CAABox::Intersect(const CRay &ray) const
{
    if (IsInside(ray.StartPoint()))
    {
	return true;
    }

    //create needed data for intersection
    float aRayDir[3], aRayStart[3], aRayRevDir[3], aVMin[3], aVMax[3];
    for (int i = 0; i < 3; ++i)
    {
	aRayDir[i] = CUtils::GetDimension(ray.Direction(), (EDimiensions)i);
	aRayStart[i] = CUtils::GetDimension(ray.StartPoint(), (EDimiensions)i);
	aVMin[i] = CUtils::GetDimension(m_vMinVertex, (EDimiensions)i);
	aVMax[i] = CUtils::GetDimension(m_vMaxVertex, (EDimiensions)i);
	aRayRevDir[i] = fabs(aRayDir[i]) > 1e-9 ? 1.0/ aRayDir[i] : 0.f;
    };

    for (int dim = 0; dim < 3; dim++)
    {
	if ((aRayDir[dim] < 0 && aRayStart[dim] < aVMin[dim]) || (aRayDir[dim] > 0 && aRayStart[dim] > aVMax[dim]))
	{
	    continue;
	}

	if (fabs(aRayDir[dim]) < 1e-9)
	{
	    continue;
	}

	const double& mul = aRayRevDir[dim];
	int u = (dim == 0) ? 1 : 0;
	int v = (dim == 2) ? 1 : 2;
	double dist, x, y;
	dist = (aVMin[dim] - aRayStart[dim]) * mul;
	if (dist < 0)
	{
	    continue;
	}
	/* (*) this is a good optimization I found out by chance. Consider the following scenario
		 *
		 *   ---+  ^  (ray)
		 *      |   \
		 * bbox |    \
		 *      |     \
		 *      |      * (camera)
		 * -----+
		 *
		 * if we're considering the walls up and down of the bbox (which belong to the same axis),
		 * the optimization in (*) says that we can skip testing with the "up" wall, if the "down"
		 * wall is behind us. The rationale for that is, that we can never intersect the "down" wall,
		 * and even if we have the chance to intersect the "up" wall, we'd be intersection the "right"
		 * wall first. So we can just skip any further intersection tests for this axis.
		 * This may seem bogus at first, as it doesn't work if the camera is inside the BBox, but then we would
		 * have quitted the function because of the inside(aRayStart) condition in the first line of the function.
		 */
	x = aRayStart[u] + aRayDir[u] * dist;
	if (aVMin[u] <= x && x <= aVMax[u])
	{
	    y = aRayStart[v] + aRayDir[v] * dist;
	    if (aVMin[v] <= y && y <= aVMax[v])
	    {
		return true;
	    }
	}
	dist = (aVMax[dim] - aRayStart[dim]) * mul;
	if (dist < 0)
	{
	    continue;
	}
	x = aRayStart[u] + aRayDir[u] * dist;
	if (aVMin[u] <= x && x <= aVMax[u])
	{
	    y = aRayStart[v] + aRayDir[v] * dist;
	    if (aVMin[v] <= y && y <= aVMax[v])
	    {
		return true;
	    }
	}
    }
    return false;
}

const QVector3D &CAABox::GetMinVertex() const
{
    return m_vMinVertex;
}

const QVector3D &CAABox::GetMaxVertex() const
{
    return m_vMaxVertex;
}

void CAABox::Split(float fWhere, CAABox &LeftBBox, CAABox &RightBBox, EDimiensions eSplitDimension) const
{
    float fLeft = CUtils::GetDimension(m_vMinVertex, eSplitDimension);
    float fRight = CUtils::GetDimension(m_vMaxVertex, eSplitDimension);
    float fSplit = (1.f - fWhere)*fLeft + fWhere*fRight;

    QVector3D vNewLeft(m_vMaxVertex);
    CUtils::SetDimension(vNewLeft, eSplitDimension, fSplit);
    LeftBBox.Set(m_vMinVertex, vNewLeft);

    QVector3D vNewRight(m_vMinVertex);
    CUtils::SetDimension(vNewRight, eSplitDimension, fSplit);
    RightBBox.Set(vNewRight, m_vMaxVertex);
}
