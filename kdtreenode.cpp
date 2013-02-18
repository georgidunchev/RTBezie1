#include "kdtreenode.h"
#include <QVector>
#include "Utils.h"
#include "settings.h"
#include <cmesh.h>
#include <main.h>
#include <raytracer.h>
#include "AABox.h"

CKDTreeNode::CKDTreeNode(QVector<int> *pTriangles, int nLevel, const CAABox &BoundingBox)
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

    QVector<int>* pLeftTriangleIndeces = new QVector<int>();
    QVector<int>* pRightTriangleIndeces = new QVector<int>();

    float fWhere;
    EDimiensions eDimention = static_cast<EDimiensions>(static_cast<int>(m_nLevel) % e_Dimension_MAX);
    if (!Separate(*m_pTriangles, pLeftTriangleIndeces, pRightTriangleIndeces, eDimention, fWhere))
    {
	return;
    }

    //splitbounding box
    CAABox LeftBBox, RightBBox;
    m_BoundingBox.Split(fWhere, LeftBBox, RightBBox, eDimention);

    m_pLeftNode = new CKDTreeNode(pLeftTriangleIndeces, m_nLevel+1, LeftBBox);
    m_pLeftNode->Process();
    m_pRightNode = new CKDTreeNode(pRightTriangleIndeces, m_nLevel+1, RightBBox);
    m_pRightNode->Process();

    m_pTriangles->clear();
}

bool CKDTreeNode::Separate(QVector<int> &AllTriangles, QVector<int> *pLeftTriangleIndeces, QVector<int> *pRightTriangleIndeces, EDimiensions eDimension, float &fBestDivide)
{
    float fPortion;
    int nMaxMinDistance = 0;
    int nBestTrianglesLeft, nBestTrianglesRight;
    for (int i = 1; i < k_nMAX_SPLITS_OF_NODE; ++i)
    {
	fPortion = 1.f/k_nMAX_SPLITS_OF_NODE;//denominator
	fPortion *= i;

	float fBBLeft = CUtils::GetDimension( m_BoundingBox.GetMinVertex(), eDimension);
	float fBBRight = CUtils::GetDimension( m_BoundingBox.GetMaxVertex(), eDimension);
	float fDivide = (fPortion-1.f)*fBBLeft + fPortion * fBBRight;

	int nTrianglesLeft = 0, nTrianglesRight = 0;

	for(int j = 0; j < AllTriangles.size(); ++j)
	{
	    float fLeft =  CUtils::GetDimension( GetMesh().GetTriangle(AllTriangles[j]).GetBoundingBox().GetMinVertex() , eDimension);
	    float fRight =  CUtils::GetDimension( GetMesh().GetTriangle(AllTriangles[j]).GetBoundingBox().GetMaxVertex() , eDimension);
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
	    fBestDivide = fPortion;
	}
    }

    if (nMaxMinDistance > 0)
    {
	pLeftTriangleIndeces->reserve(nBestTrianglesLeft);
	pRightTriangleIndeces->reserve(nBestTrianglesRight);
	for (int j = 0; j < AllTriangles.size(); ++j)
	{
	    float fLeft =  CUtils::GetDimension( GetMesh().GetTriangle(AllTriangles[j]).GetBoundingBox().GetMinVertex() , eDimension);
	    float fRight =  CUtils::GetDimension( GetMesh().GetTriangle(AllTriangles[j]).GetBoundingBox().GetMaxVertex() , eDimension);
	    if (fLeft <= fBestDivide)
	    {
		pLeftTriangleIndeces->push_back(AllTriangles[j]);
	    }
	    if (fRight >= fBestDivide)
	    {
		pRightTriangleIndeces->push_back(AllTriangles[j]);
	    }
	}
	return true;
    }

    return false;
}

bool CKDTreeNode::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    // discard intersection in ray does not cross the BBox of the node
    if ( !m_BoundingBox.Intersect(ray) )
    {
	return false;
    }
    //end criteria - the node is a leaf and we intersect the contained triangles
    if (!m_pLeftNode || !m_pRightNode)
    {
	if (m_pTriangles->size() <= 0)
	{
	    return false;
	}

	return GetMesh().Intersect(ray,intersectionInfo, *m_pTriangles, &m_BoundingBox);
    }
    else
    {

	CIntersactionInfo intersectionInfoLeft(intersectionInfo);
	CIntersactionInfo intersectionInfoRight(intersectionInfo);

	bool bIntersectLeft = m_pLeftNode->Intersect(ray, intersectionInfoLeft);
	bool bIntersectRight = m_pLeftNode->Intersect(ray, intersectionInfoRight);

	if (!bIntersectLeft && !bIntersectRight)
	{
	    return false;
	}
	else if(bIntersectLeft)
	{
	    intersectionInfo = intersectionInfoLeft;
	    return true;
	}
	else if(bIntersectRight)
	{
	    intersectionInfo = intersectionInfoRight;
	    return true;
	}
	else
	{
	    // two intersection, choose the closest one
	    if (intersectionInfoLeft.m_fDistance < intersectionInfoRight.m_fDistance)
	    {
		intersectionInfo = intersectionInfoLeft;
	    }
	    else
	    {
		intersectionInfo = intersectionInfoRight;
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
