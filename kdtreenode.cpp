#include "kdtreenode.h"
#include <vector>
#include "Utils.h"
#include "settings.h"
#include "cmesh.h"
#include "main.h"
#include "raytracer.h"
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
    if (m_pLeftNode)
    {
        delete m_pLeftNode;
    }
    if (m_pRightNode)
    {
        delete m_pRightNode;
    }
    delete m_pTriangles;
}

int CKDTreeNode::Process()
{
	// divide the input triangles;
	// create two child nodes using the separated triangles;
	// clear the input triangles' array;

	if (m_nLevel >= k_nMAX_LEVEL_OF_TREE
		|| m_pTriangles->size() <= k_nMIN_NUMBER_OF_TRIANGLES_TO_SPLIT)
	{
        return 1;
	}

	std::vector<CSubTriangle*>* pLeftTriangles = new std::vector<CSubTriangle*>();
	std::vector<CSubTriangle*>* pRightTriangles = new std::vector<CSubTriangle*>();

	float fWhere;
	CVector3DF::EDimiensions eDimention = static_cast<CVector3DF::EDimiensions>(static_cast<int>(m_nLevel) % CVector3DF::e_Dimension_MAX);

	if (!Separate(*m_pTriangles, pLeftTriangles, pRightTriangles, eDimention, fWhere))
	{
        return 1;
	}

	//splitbounding box
	CAABox LeftBBox, RightBBox;
	m_BoundingBox.Split(fWhere, LeftBBox, RightBBox, eDimention);

	//qDebug() << "Split level:" << m_nLevel << "Left" << pLeftTriangles->size() << "Right"<< pRightTriangles->size();

    int nLeafs = 0;
	m_pLeftNode = new CKDTreeNode(pLeftTriangles, m_nLevel+1, LeftBBox);
    nLeafs += m_pLeftNode->Process();
	m_pRightNode = new CKDTreeNode(pRightTriangles, m_nLevel+1, RightBBox);
    nLeafs += m_pRightNode->Process();

	m_pTriangles->clear();
    return nLeafs;
}

bool CKDTreeNode::Separate(	std::vector<CSubTriangle*>& AllTriangles,
							std::vector<CSubTriangle*>* pLeftTriangles,
							std::vector<CSubTriangle*>*pRightTriangles,
							CVector3DF::EDimiensions eDimension, float &fBestPortion)
{
	float fPortion, fBestDivide;
	int nMaxMinDistance = 0;
	int nBestTrianglesLeft, nBestTrianglesRight;

	for (int i = 1; i < k_nMAX_SPLITS_OF_NODE; ++i)
	{
		fPortion = 1.f/k_nMAX_SPLITS_OF_NODE;//denominator
		fPortion *= i;

		float fBBLeft = m_BoundingBox.GetMinVertex().GetDimension(eDimension);
		float fBBRight = m_BoundingBox.GetMaxVertex().GetDimension(eDimension);
		float fDivide = (1.f - fPortion)*fBBLeft + fPortion * fBBRight;

		int nTrianglesLeft = 0, nTrianglesRight = 0;

		for(uint j = 0; j < AllTriangles.size(); ++j)
		{
		    float fLeft =  AllTriangles[j]->GetBoundingBox().GetMinVertex().GetDimension(eDimension);
			float fRight = AllTriangles[j]->GetBoundingBox().GetMaxVertex().GetDimension(eDimension);
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

		for (uint j = 0; j < AllTriangles.size(); ++j)
		{
			float fLeft =  AllTriangles[j]->GetBoundingBox().GetMinVertex().GetDimension(eDimension);
			float fRight =  AllTriangles[j]->GetBoundingBox().GetMaxVertex().GetDimension(eDimension);
		
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
//    intersectionInfo.m_nAABBTime = intersectionInfo.m_nObjTime = 0;
//    intersectionInfo.m_nAABBIntersections = intersectionInfo.m_nBezierIntersections = 0;

//    qint64 nAABBTime;
    {
        QElapsedTimer timer;
        timer.start();
        // discard intersection in ray does not cross the BBox of the node
        bool b = m_BoundingBox.Intersect(ray, bDebug);
        intersectionInfo.m_nAABBIntersections += 1;
        intersectionInfo.m_nAABBTime += timer.nsecsElapsed();
//        nAABBTime = timer.nsecsElapsed();

        if (!b)
        {

//            intersectionInfo.m_nAABBTime = nAABBTime;
            return false;
        }
    }
	
	//end criteria - the node is a leaf and we intersect the contained triangles
	if (!m_pLeftNode || !m_pRightNode)
	{
        intersectionInfo.m_nBezierIntersections += m_pTriangles->size();
        QElapsedTimer timer2;
        timer2.start();
		bool bIntersect = CSubTriangle::IntersectSubdevidedTriangles(ray, intersectionInfo, *m_pTriangles, bDebug);
        intersectionInfo.m_nObjTime += timer2.nsecsElapsed();
//        intersectionInfo.m_nAABBTime += nAABBTime;
//        intersectionInfo.m_nAABBIntersections = 1;

//        if (!bIntersect && GetSettings()->m_bShowKDTtee)
//        {
//            return m_BoundingBox.Intersect(ray, intersectionInfo, bDebug);
//        }
		return bIntersect;
	}
	else
	{
//        CIntersactionInfo intersectionInfoLeft(intersectionInfo);
        bool bIntersectLeft = m_pLeftNode->Intersect(ray, intersectionInfo, bDebug);

//        CIntersactionInfo intersectionInfoRight(intersectionInfo);
        bool bIntersectRight = m_pRightNode->Intersect(ray, intersectionInfo, bDebug);


//        intersectionInfoLeft.m_nAABBIntersections   = intersectionInfoRight.m_nAABBIntersections
//                = 1 + intersectionInfoLeft.m_nAABBIntersections + intersectionInfoRight.m_nAABBIntersections;

//        intersectionInfoLeft.m_nAABBTime            = intersectionInfoRight.m_nAABBTime
//                = nAABBTime + intersectionInfoLeft.m_nAABBTime + intersectionInfoRight.m_nAABBTime;

//        intersectionInfoLeft.m_nBezierIntersections = intersectionInfoRight.m_nBezierIntersections
//                = intersectionInfoLeft.m_nBezierIntersections + intersectionInfoRight.m_nBezierIntersections;

//        intersectionInfoLeft.m_nObjTime             = intersectionInfoRight.m_nObjTime
//                = intersectionInfoLeft.m_nObjTime + intersectionInfoRight.m_nObjTime;

		if (!bIntersectLeft && !bIntersectRight)
		{
			return false;
		}
		else
		{
			float fR = 0.0f, fG = 0.0f, fB = 0.0f;
			static const float fColorIncrement = 1.0f / static_cast<float>(k_nMAX_LEVEL_OF_TREE);

            if(!bIntersectRight)
			{
				fR += fColorIncrement;
//				intersectionInfo = intersectionInfoLeft;
			}
            else //if(!bIntersectLeft && bIntersectRight)
			{
				fB += fColorIncrement;
//				intersectionInfo = intersectionInfoRight;
			}
//			else
//			{
//				// two intersection, choose the closest one
//				if (intersectionInfoLeft.m_fDistance < intersectionInfoRight.m_fDistance)
//				{
//					fR += fColorIncrement;
////					intersectionInfo = intersectionInfoLeft;
//				}
//				else
//				{
//					fB += fColorIncrement;
////					intersectionInfo = intersectionInfoRight;
//				}
//			}

            if (GetSettings()->m_bShowKDTtee)
			{
				intersectionInfo.color += CColor(fR, fG, fB);
			}

			return true;
		}
    }
}

CMesh &CKDTreeNode::GetMesh()
{
	return GetRaytracer()->GetMesh();
}
