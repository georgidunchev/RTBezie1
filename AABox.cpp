#include <QDebug>

#include "AABox.h"
#include "ray.h"
#include "vector3df.h"
#include "qmath.h"
#include "Utils.h"
#include "settings.h"
#include "vertex.h"
#include "intersactioninfo.h"

CAABox::CAABox()
	: m_vMaxVertex(CVector3DF(k_fMIN,k_fMIN,k_fMIN))
	, m_vMinVertex(CVector3DF(k_fMAX,k_fMAX,k_fMAX))
{
}

CAABox::CAABox(const CVector3DF &vMinVertex, const CVector3DF &vMaxVertex)
{
	m_vMinVertex = vMinVertex;
	m_vMaxVertex = vMaxVertex;
}

void CAABox::Set(const CVector3DF &vMinVertex, const CVector3DF &vMaxVertex)
{
	m_vMinVertex = vMinVertex;
	m_vMaxVertex = vMaxVertex;
}

void CAABox::Reset()
{
	m_vMaxVertex = CVector3DF(k_fMIN,k_fMIN,k_fMIN);
	m_vMinVertex = CVector3DF(k_fMAX,k_fMAX,k_fMAX);
}

/// Checks if a point is inside the bounding box (borders-inclusive)
bool CAABox::IsInside(const CVector3DF &vPoint) const
{
	bool bX = m_vMinVertex.X() - k_fSMALL <= vPoint.X() && vPoint.X() <= m_vMaxVertex.X() + k_fSMALL;
	bool bY = m_vMinVertex.Y() - k_fSMALL  <= vPoint.Y() && vPoint.Y() <= m_vMaxVertex.Y() + k_fSMALL;
	bool bZ = m_vMinVertex.Z() - k_fSMALL  <= vPoint.Z() && vPoint.Z() <= m_vMaxVertex.Z() + k_fSMALL;
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

void CAABox::AddPoint(const CVector3DF &vPoint)
{
	for(int i = CVector3DF::e_Dimension_X; i < CVector3DF::e_Dimension_MAX; ++i)
	{
	    const CVector3DF::EDimiensions eDim = (CVector3DF::EDimiensions)i;
		const float fMinVDir( m_vMinVertex.GetDimension( eDim ) );
		const float fMaxVDir( m_vMaxVertex.GetDimension( eDim ) );
		const float fPointDir( vPoint.GetDimension( eDim ) );

		if(fMinVDir > fPointDir)
		{
		    m_vMinVertex.SetDimension(eDim, fPointDir);
		}

		if(fMaxVDir < fPointDir)
		{
		    m_vMaxVertex.SetDimension(eDim, fPointDir);
		}
	}
}

bool CAABox::Intersect(const CRay &ray, CIntersactionInfo& io_IntersectionInfo, bool bDebug) const
{
	return Intersect_Test(ray, io_IntersectionInfo, bDebug);
}

bool CAABox::Intersect(const CRay &ray, bool bDebug) const
{
	if (IsInside(ray.StartPoint()))
	{
		return true;
	}

	//if (bDebug)
	//{
	//	CIntersactionInfo IntersectionInfo;
	//	return Intersect_Test(ray, IntersectionInfo, bDebug);
	//}

	//create needed data for intersection
	float aRayDir[3], aRayStart[3], aRayRevDir[3], aVMin[3], aVMax[3];
	for (int i = 0; i < 3; ++i)
	{
	    const CVector3DF::EDimiensions eDim = (CVector3DF::EDimiensions) i;
		aRayDir[i] = ray.Direction().GetDimension(eDim);
		aRayStart[i] = ray.StartPoint().GetDimension(eDim);
		aVMin[i] = m_vMinVertex.GetDimension(eDim);
		aVMax[i] = m_vMaxVertex.GetDimension(eDim);
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
		
		const float fMinX = aVMin[u] - k_fSMALL;
		const float fMaxX = aVMax[u] + k_fSMALL;
			
		const float fMinY = aVMin[v] - k_fSMALL;
		const float fMaxY = aVMax[v] + k_fSMALL;

		if (fMinX <= x && x <= fMaxX)
		{
			y = aRayStart[v] + aRayDir[v] * dist;
			if (fMinY <= y && y <= fMaxY)
			{
				if ( bDebug )
				{
//					qDebug() << "A Success Dim: "<< (EDimiensions)dim <<"X: "<< fMinX << x << fMaxX << "Y: " << fMinY << y << fMaxY;
				}
				return true;
			}
			if ( bDebug )
			{
//				qDebug() << "A Fail Dim: "<< (EDimiensions)dim <<"X: "<< fMinX << x << fMaxX << "Y: " << fMinY << y << fMaxY;
			}
		}

		dist = (aVMax[dim] - aRayStart[dim]) * mul;
		if (dist < 0)
		{
			continue;
		}

		x = aRayStart[u] + aRayDir[u] * dist;

		if (fMinX <= x && x <= fMaxX)
		{
			y = aRayStart[v] + aRayDir[v] * dist;
			if (fMinY <= y && y <= fMaxY)
			{
				if ( bDebug )
				{
//					qDebug() << "B Success Dim: "<< (EDimiensions)dim <<"X: "<< fMinX << x << fMaxX << "Y: " << fMinY << y << fMaxY;
				}
				return true;
			}
			if ( bDebug )
			{
//				qDebug() << "B Fail Dim: "<< (EDimiensions)dim <<"X: "<< fMinX << x << fMaxX << "Y: " << fMinY << y << fMaxY;
			}
		}
	}
	return false;
}

const CVector3DF &CAABox::GetMinVertex() const
{
	return m_vMinVertex;
}

const CVector3DF &CAABox::GetMaxVertex() const
{
	return m_vMaxVertex;
}

void CAABox::Split(float fWhere, CAABox &LeftBBox, CAABox &RightBBox, CVector3DF::EDimiensions eSplitDimension) const
{
	float fLeft = m_vMinVertex.GetDimension(eSplitDimension);
	float fRight = m_vMaxVertex.GetDimension(eSplitDimension);
	float fSplit = (1.f - fWhere)*fLeft + fWhere*fRight;

	CVector3DF vNewLeft(m_vMaxVertex);
	vNewLeft.SetDimension(eSplitDimension, fSplit + k_fSMALL);
	LeftBBox.Set(m_vMinVertex, vNewLeft);

	CVector3DF vNewRight(m_vMinVertex);
	vNewRight.SetDimension(eSplitDimension, fSplit - k_fSMALL);
	RightBBox.Set(vNewRight, m_vMaxVertex);
}

bool CAABox::Intersect_Test(const CRay &ray, CIntersactionInfo& io_IntersectionInfo, bool bDebug) const
{
//	if (IsInside(ray.StartPoint()))
//	{
//		return true;
//	}

//	std::vector<CVector3DF> vVerteces;
//	vVerteces.reserve(8);
//	vVerteces.push_back(m_vMinVertex);
//	vVerteces.push_back(CVector3DF(m_vMaxVertex.x(), m_vMinVertex.y(), m_vMinVertex.z()));
//	vVerteces.push_back(CVector3DF(m_vMaxVertex.x(), m_vMinVertex.y(), m_vMaxVertex.z()));
//	vVerteces.push_back(CVector3DF(m_vMinVertex.x(), m_vMinVertex.y(), m_vMaxVertex.z()));

//	vVerteces.push_back(CVector3DF(m_vMinVertex.x(), m_vMaxVertex.y(), m_vMinVertex.z()));
//	vVerteces.push_back(CVector3DF(m_vMaxVertex.x(), m_vMaxVertex.y(), m_vMinVertex.z()));
//	vVerteces.push_back(m_vMaxVertex);
//	vVerteces.push_back(CVector3DF(m_vMinVertex.x(), m_vMaxVertex.y(), m_vMaxVertex.z()));

//	struct bla2
//	{
//		bla2(int ix, int iy, int iz)
//			:x(ix)
//			,y(iy)
//			,z(iz)
//		{}
//		int x, y, z;
//	};

//	std::vector<bla2> bla;
//	bla.reserve(6);
//	bla.push_back(bla2(0,1,2));
//	bla.push_back(bla2(0,2,3));
//	bla.push_back(bla2(0,1,5));
//	bla.push_back(bla2(0,5,4));
//	bla.push_back(bla2(1,2,6));
//	bla.push_back(bla2(1,6,5));
//	bla.push_back(bla2(0,3,7));
//	bla.push_back(bla2(0,7,4));
//	bla.push_back(bla2(3,2,6));
//	bla.push_back(bla2(3,6,7));
//	bla.push_back(bla2(4,5,6));
//	bla.push_back(bla2(4,6,7));

//	for (int i = 0; i < bla.size(); ++i)
//	{
//		if (CUtils::IntersectTriangle(ray, io_IntersectionInfo, vVerteces[bla[i].x], vVerteces[bla[i].y], vVerteces[bla[i].z]))
//		{
//			return true;
//		}
//	}
	return false;
}
