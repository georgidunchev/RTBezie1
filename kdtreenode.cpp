#include "kdtreenode.h"
#include <vector>
#include "Utils.h"
#include "settings.h"
#include <cmesh.h>
#include <main.h>
#include <raytracer.h>
#include "AABox.h"
#include <qdebug.h>

CKDTreeNode::CKDTreeNode(std::vector<CSubTriangle*>*pTriangles, int nLevel, const CAABox &BoundingBox)
	: m_pTriangles(pTriangles)
	, m_nLevel(nLevel)
	, m_pLeftNode(NULL)
	, m_pRightNode(NULL)
	, m_BoundingBox(BoundingBox)
{
}

CKDTreeNode::~CKDTreeNode()
{
	CUtils::SafeDel(m_pLeftNode);
	CUtils::SafeDel(m_pRightNode);
}

void CKDTreeNode::Process()
{
	// divide the input triangles;
	// create two child nodes using the separated triangles;
	// clear the input triangles' array;

	if (m_nLevel >= k_nMAX_LEVEL_OF_TREE
		|| m_pTriangles->size() <= k_nMIN_NUMBER_OF_TRIANGLES_TO_SPLIT)
	{
		return;
	}

	std::vector<CSubTriangle*>* pLeftTriangles = new std::vector<CSubTriangle*>();
	std::vector<CSubTriangle*>* pRightTriangles = new std::vector<CSubTriangle*>();

	float fWhere;
	EDimiensions eDimention = static_cast<EDimiensions>(static_cast<int>(m_nLevel) % e_Dimension_MAX);

	if (!Separate(*m_pTriangles, pLeftTriangles, pRightTriangles, eDimention, fWhere))
	{
		return;
	}

	//splitbounding box
	CAABox LeftBBox, RightBBox;
	m_BoundingBox.Split(fWhere, LeftBBox, RightBBox, eDimention);

	qDebug() << "Split level:" << m_nLevel << "Left" << pLeftTriangles->size() << "Right"<< pRightTriangles->size();

	m_pLeftNode = new CKDTreeNode(pLeftTriangles, m_nLevel+1, LeftBBox);
	m_pLeftNode->Process();
	m_pRightNode = new CKDTreeNode(pRightTriangles, m_nLevel+1, RightBBox);
	m_pRightNode->Process();

	m_pTriangles->clear();
}

bool CKDTreeNode::Separate(	std::vector<CSubTriangle*>& AllTriangles,
							std::vector<CSubTriangle*>* pLeftTriangles,
							std::vector<CSubTriangle*>*pRightTriangles,
							EDimiensions eDimension, float &fBestPortion)
{
	float fPortion, fBestDivide;
	int nMaxMinDistance = 0;
	int nBestTrianglesLeft, nBestTrianglesRight;

	for (int i = 1; i < k_nMAX_SPLITS_OF_NODE; ++i)
	{
		fPortion = 1.f/k_nMAX_SPLITS_OF_NODE;//denominator
		fPortion *= i;

		float fBBLeft = CUtils::GetDimension( m_BoundingBox.GetMinVertex(), eDimension);
		float fBBRight = CUtils::GetDimension( m_BoundingBox.GetMaxVertex(), eDimension);
		float fDivide = (1.f - fPortion)*fBBLeft + fPortion * fBBRight;

		int nTrianglesLeft = 0, nTrianglesRight = 0;

		for(int j = 0; j < AllTriangles.size(); ++j)
		{
			float fLeft =  CUtils::GetDimension( AllTriangles[j]->GetBoundingBox().GetMinVertex() , eDimension);
			float fRight =  CUtils::GetDimension( AllTriangles[j]->GetBoundingBox().GetMaxVertex() , eDimension);
			if (fLeft <= fDivide)
			{
				nTrianglesLeft++;
			}
			if (fRight >= fDivide)
			{
				nTrianglesRight++;
			}
		}

		int nMin = qMin(nTrianglesLeft, nTrianglesRight);
		
		if (nMin > nMaxMinDistance)
		{
			nMaxMinDistance = nMin;
			nBestTrianglesLeft = nTrianglesLeft;
			nBestTrianglesRight = nTrianglesRight;
			fBestPortion = fPortion;
			fBestDivide = fDivide;
		}
	}

	if (nMaxMinDistance > 0)
	{
		pLeftTriangles->reserve(nBestTrianglesLeft);
		pRightTriangles->reserve(nBestTrianglesRight);

		for (int j = 0; j < AllTriangles.size(); ++j)
		{
			float fLeft =  CUtils::GetDimension( AllTriangles[j]->GetBoundingBox().GetMinVertex() , eDimension);
			float fRight =  CUtils::GetDimension( AllTriangles[j]->GetBoundingBox().GetMaxVertex() , eDimension);
		
			if (fLeft <= fBestDivide)
			{
				pLeftTriangles->push_back(AllTriangles[j]);
			}
			
			if (fRight >= fBestDivide)
			{
				pRightTriangles->push_back(AllTriangles[j]);
			}
		}
		return true;
	}

	return false;
}

bool CKDTreeNode::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug)
{
	// discard intersection in ray does not cross the BBox of the node
	if ( !m_BoundingBox.Intersect(ray, bDebug) )
	{
		return false;
	}
	
	//end criteria - the node is a leaf and we intersect the contained triangles
	if (!m_pLeftNode || !m_pRightNode)
	{
		bool bIntersect = CTriangle::IntersectSubdevidedTriangles(ray, intersectionInfo, *m_pTriangles);
		//bool bIntersect = CTriangle::Intersect(ray, intersectionInfo, *m_pTriangles);
		if (!bIntersect && k_bSHOW_KDTREE)
		{
			return m_BoundingBox.Intersect(ray, intersectionInfo, bDebug);
		}
		return bIntersect;
	}
	else
	{
		if (bDebug)
		{
			qDebug() << "Left";
		}

		CIntersactionInfo intersectionInfoLeft(intersectionInfo);
		bool bIntersectLeft = m_pLeftNode->Intersect(ray, intersectionInfoLeft, bDebug);
		
		if (bDebug)
		{
			qDebug() << "Right";
		}

		CIntersactionInfo intersectionInfoRight(intersectionInfo);
		bool bIntersectRight = m_pRightNode->Intersect(ray, intersectionInfoRight, bDebug);

		if (!bIntersectLeft && !bIntersectRight)
		{
			return false;
		}
		else
		{
			float fR = 0.0f, fG = 0.0f, fB = 0.0f;
			static const float fColorIncrement = 1.0f / static_cast<float>(k_nMAX_LEVEL_OF_TREE);

			if(bIntersectLeft && !bIntersectRight)
			{
				fR += fColorIncrement;
				intersectionInfo = intersectionInfoLeft;
			}
			else if(!bIntersectLeft && bIntersectRight)
			{
				fB += fColorIncrement;
				intersectionInfo = intersectionInfoRight;
			}
			else
			{
				// two intersection, choose the closest one
				if (intersectionInfoLeft.m_fDistance < intersectionInfoRight.m_fDistance)
				{
					fR += fColorIncrement;
					intersectionInfo = intersectionInfoLeft;
				}
				else
				{
					fB += fColorIncrement;
					intersectionInfo = intersectionInfoRight;
				}
			}

			if (k_bSHOW_KDTREE)
			{
				intersectionInfo.color += CColor(fR, fG, fB);
			}

			return true;
		}
	}
	return false;
}

CMesh &CKDTreeNode::GetMesh()
{
	return GetRaytracer()->GetMesh();
}
