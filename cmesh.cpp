#include <qalgorithms.h>
#include "cmesh.h"
#include "QTextStream"
#include <QDebug>
#include <QFile>
#include "kdtreenode.h"
#include "AABox.h"
#include <main.h>
#include <raytracer.h>
#include <settings.h>
#include "SubTriangle.h"

CMesh::CMesh(QObject *parent)
    : QObject(parent)
    , m_pRoot(NULL)
{

}

void CMesh::Load(const QString& strInputFileName)
{
    int nSteps = 5;
    emit sigLoadingStarted(nSteps);

    int nCrntStep = 1;

    ReadFromFile(strInputFileName);
    emit sigLoadingStepDone( nCrntStep++ );

    BuildVertexData();
    emit sigLoadingStepDone( nCrntStep++ );

    //BuildAdjacency();

    BuildBezierTriangles();
    emit sigLoadingStepDone( nCrntStep++ );

    MakeBoundingBox(); //needs all triangle/subtriangles to be generated
    emit sigLoadingStepDone( nCrntStep++ );

    GenerateKDTree(); // depends on the bounding box
    emit sigLoadingFinished();
}

void CMesh::ReadFromFile(const QString &strInputFileName)
{
    QFile file(strInputFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
	return;
    }

    QTextStream inputStream(&file);
    QString strLine;

    m_aVertices.clear();
    m_aTriangles.clear();

    m_aVertices.push_back( CVertex() );

    QString strMarker;
    while(!inputStream.atEnd())
    {
	strLine = inputStream.readLine();
	QTextStream lineStream(&strLine);
	lineStream >> strMarker;
	//we add a vertex
	if (strMarker == "#")
	{
	    continue;
	}
	else if (strMarker == "v" )
	{
	    std::vector<qreal> aCoords;
	    for (uint i = 0; i < 3; ++i)
	    {
		lineStream >> strMarker;
		if(strMarker.isEmpty())
		{
		    //strReturnMessage << "Error: Number of Elements in Vertex: " << m_aVertices.size();
		    break;
		}

		aCoords.push_back(strMarker.toDouble());
	    }

	    m_aVertices.push_back(CVertex(QVector3D(aCoords[0], aCoords[1], aCoords[2]), m_aVertices.size()));
	}
	else if (strMarker == "f" )
	{
	    std::vector<int> aVertIDs;
	    lineStream >> strMarker;
	    int i = 0;

	    while(!strMarker.isEmpty())
	    {
		i++;
		aVertIDs.push_back(strMarker.toInt());
		lineStream >> strMarker;
	    }

	    if (i < 3)
	    {
		//strReturnMessage << "Error: Number of Indecis in Face: " << m_aTriangles.size();
		break;
	    }

	    if ( i > 3)
	    {
		qDebug()<<"Face splitted to multiple triangles";
	    }

	    for ( uint i = 2; i < aVertIDs.size(); ++i)
	    {
		m_aTriangles.push_back(new CTriangle(m_aVertices, aVertIDs[0], aVertIDs[i-1], aVertIDs[i], m_aTriangles.size()));
	    }
	}
    }
    file.close();
}

void CMesh::MakeBoundingBox() 
{
    // make bounding box after we have generated all triangles, subt triangles etc
    m_BoundingBox.Reset();
    for (uint i = 0; i < m_aTriangles.size(); ++i)
    {
	m_aTriangles[i]->MakeBoundingBox();
	m_BoundingBox.AddPoint(m_aTriangles[i]->GetBoundingBox().GetMinVertex());
	m_BoundingBox.AddPoint(m_aTriangles[i]->GetBoundingBox().GetMaxVertex());
    }
}

bool CMesh::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug)
{
    bool bResult = false;
    if (k_bUSE_KDTREE)
    {
	bool bIntersect = IntersectKDTree(ray, intersectionInfo, bDebug);
	bResult = bIntersect;
	if (bIntersect && GetRaytracer()->IsHighQuality())
	{
	    intersectionInfo.m_fDistance = k_fMAX;
	    std::vector<CSubTriangle*> aSubTriangles;
	    aSubTriangles.push_back(intersectionInfo.pSubTriangle);
	    bResult = CTriangle::Intersect(ray, intersectionInfo, aSubTriangles);
	}
    }
    else
    {
	std::vector<int> aTriangles(GetPrimitives()->size());
	for (uint i = 0; i < aTriangles.size(); ++i)
	{
	    aTriangles[i] = i;
	}
	bResult = Intersect(ray, intersectionInfo, aTriangles, NULL, bDebug);
    }

    if(bResult && intersectionInfo.m_bInitialRay)
    {
	int i = 0;
	QVector3D vIntersection = intersectionInfo.m_vIntersectionPoint;
	QVector3D vLIghtDirection = GetRaytracer()->GetLightScene().GetLight(i).GetPosition() - vIntersection;
	vLIghtDirection.normalize();
	vIntersection += vLIghtDirection * k_fSMALL;

	//CRay LightRay(GetLightScene().GetLight(i).GetPosition(), vLIghtDirection);
	CRay LightRay(vIntersection, vLIghtDirection);

	float fDistToLight = vLIghtDirection.length();

	CIntersactionInfo LightIntersectionInfo;
	LightIntersectionInfo.m_bInitialRay = false;
	if (Intersect(LightRay, LightIntersectionInfo))
	{
	    //if the first intersection is the intersection point
	    if (LightIntersectionInfo.m_fDistance < fDistToLight - k_fSMALL)
	    {
		CColor colorForLight(0.f, 0.f, 0.f);
		intersectionInfo.color = colorForLight;
	    }
	}
    }

    return bResult;
}

bool CMesh::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<int>& aTriangles, CAABox* pBBox, bool bDebug)
{
    bool bIntersected = false;

    for (uint i = 0; i < aTriangles.size(); ++i)
    {
	CTriangle* Triangle = GetPrimitive(aTriangles[i]);
	CIntersactionInfo LastIntersection;

	if (bDebug)
	{
	    qDebug() << "Intersecting Triangle: " << i;
	}

	if ( Triangle->Intersect(ray, LastIntersection, bDebug) )
	{
	    if (pBBox) // if bounding box is supplied, discard all intersection outside it
	    {
		QVector3D vIntersection = CUtils::GetPointAtDistance(ray, LastIntersection.m_fDistance);
		if (!pBBox->IsInside(vIntersection))
		{
		    continue;
		}
	    }
	    if ( !bIntersected )
	    {
		intersectionInfo = LastIntersection;
		bIntersected = true;
	    }
	    else
	    {
		if (intersectionInfo.m_fDistance > LastIntersection.m_fDistance)
		{
		    intersectionInfo = LastIntersection;
		}
	    }
	}
    }
    return bIntersected;
}

bool CMesh::IntersectKDTree(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug)
{
    if (bDebug)
    {
	qDebug()<<"Traverse KDTree";
    }
    return m_pRoot->Intersect(ray, intersectionInfo, bDebug);
}

std::vector<CTriangle*> *CMesh::GetPrimitives()
{
    //if (GetSettings()->GetIntersectBezier())
    //{
    //	return &m_aBezierTriangles;
    //}
    //else
    {
	return &m_aTriangles;
    }
}

std::vector<CTriangle *> &CMesh::GetTriangles()
{
    return m_aTriangles;
}

CTriangle *CMesh::GetPrimitive(int n)
{
    std::vector<CTriangle*>& tmpVector = *GetPrimitives();
    return tmpVector[n];
}

const std::vector<CVertex> &CMesh::GetVertices() const
{
    return m_aVertices;
}

std::vector<CVertex> &CMesh::Vertices()
{
    return m_aVertices;
}

const SAdjacencyOfTriangle &CMesh::GetAdjacentTriangles(int n)
{
    return m_aAdjacentTriangles[n];
}

CBezierTriangle *CMesh::GetBezierTriangle(int n)
{
    return static_cast<CBezierTriangle*>(m_aBezierTriangles[n]);
}

void CMesh::GenerateKDTree()
{
    //initialize an array containing all triangles
    int nNumberOfSubTriangles = CUtils::PowerOf2(GetSettings()->GetNofSubdivisions());
    int nNumberOfPrimitives = GetPrimitives()->size();

    std::vector<CSubTriangle*>* pAllSubTriangles = new std::vector<CSubTriangle*>;
    pAllSubTriangles->reserve(nNumberOfSubTriangles * nNumberOfPrimitives);

    for (int j = 0; j < nNumberOfPrimitives; j++)
    {
	for (int i = 0; i < nNumberOfSubTriangles; ++i)
	{
	    pAllSubTriangles->push_back(GetPrimitive(j)->GetSubTriangle(i));
	}
    }
    CUtils::SafeDel(m_pRoot);
    m_pRoot = new CKDTreeNode(pAllSubTriangles, 0, m_BoundingBox);
    m_pRoot->Process();
}

bool CMesh::CompareBB(const CSortedBBEntry &s1, const CSortedBBEntry &s2)
{
    const CAABox& BB1 = m_aTriangles[s1.m_nTriangleId]->GetBoundingBox();
    const QVector3D& vBB1( s1.m_bStart ? BB1.GetMinVertex() : BB1.GetMaxVertex() );
    float fValue1 = CUtils::GetDimension(vBB1, m_eSortingDimention);

    const CAABox& BB2 = m_aTriangles[s2.m_nTriangleId]->GetBoundingBox();
    const QVector3D& vBB2( s2.m_bStart ? BB2.GetMinVertex() : BB2.GetMaxVertex() );
    float fValue2 = CUtils::GetDimension(vBB2, m_eSortingDimention);

    return fValue1 < fValue2;
}

int CMesh::GetKDTreeNextID()
{
    return 1;
}

void CMesh::SortBBoxes()
{
    int nSize = m_aTriangles.size() * 2;
    for(EDimiensions i = e_Dimension_X; i < e_Dimension_MAX; i = (EDimiensions)((int)i + 1) )
    {
	m_aSortedBBoxes[i].clear();
	m_aSortedBBoxes[i].resize(nSize);
	for (uint j = 0; j < m_aSortedBBoxes[i].size(); ++j)
	{
	    m_aSortedBBoxes[i][j].m_bStart = j%2==0;
	    m_aSortedBBoxes[i][j].m_nTriangleId = j/2;
	}
    }

    qSort(m_aSortedBBoxes[e_Dimension_X].begin(), m_aSortedBBoxes[e_Dimension_X].end(), CSortedBBEntry::compare_X);
    qSort(m_aSortedBBoxes[e_Dimension_Y].begin(), m_aSortedBBoxes[e_Dimension_Y].end(), CSortedBBEntry::compare_Y);
    qSort(m_aSortedBBoxes[e_Dimension_Z].begin(), m_aSortedBBoxes[e_Dimension_Z].end(), CSortedBBEntry::compare_Z);

}

bool CSortedBBEntry::compare_X(const CSortedBBEntry &that, const CSortedBBEntry &other)
{
    return compare(that, other, e_Dimension_X);
}

bool CSortedBBEntry::compare_Y(const CSortedBBEntry &that, const CSortedBBEntry &other)
{
    return compare(that, other, e_Dimension_Y);
}

bool CSortedBBEntry::compare_Z(const CSortedBBEntry &that, const CSortedBBEntry &other)
{
    return compare(that, other, e_Dimension_Z);
}

bool CSortedBBEntry::compare(const CSortedBBEntry &that, const CSortedBBEntry &other, EDimiensions eSortingDimention)
{
    CMesh& mesh = GetRaytracer()->GetMesh();

    const CAABox& BB1 = mesh.GetPrimitive(that.m_nTriangleId)->GetBoundingBox();
    const QVector3D& vBB1( that.m_bStart ? BB1.GetMinVertex() : BB1.GetMaxVertex() );
    float fValue1 = CUtils::GetDimension(vBB1, eSortingDimention);

    const CAABox& BB2 = mesh.GetPrimitive(other.m_nTriangleId)->GetBoundingBox();
    const QVector3D& vBB2( other.m_bStart ? BB2.GetMinVertex() : BB2.GetMaxVertex() );
    float fValue2 = CUtils::GetDimension(vBB2, eSortingDimention);

    return fValue1 < fValue2;
}

void CMesh::BuildVertexData()
{
    for (uint i = 0; i < m_aTriangles.size(); ++i)
    {
	for (int j = 0; j < 3; ++j)
	{
	    m_aTriangles[i]->GetVertex(j).Normal_AddNormal(m_aTriangles[i]->Normal());
	}
    }
    for (uint i = 0; i < m_aVertices.size(); ++i)
    {
	m_aVertices[i].Normal_Normalize();
    }
}

void CMesh::BuildAdjacency()
{
    m_nTrianglesWithCompleteAdjacency = 0;
    uint nSize = m_aTriangles.size();
    m_aAdjacentTriangles.clear();
    m_aAdjacentTriangles.resize(nSize);
    for (uint i = 0; i < nSize; ++i)
    {
	for (uint j = 0; j < 3; ++j)
	{
	    //if (!m_aAdjacentTriangles[i].Complete(j))
	    {
		if (FindAdjacentTriangle(i,j) != -1)
		{
		    m_aAdjacentTriangles[i].SetComplete(j);
		}
	    }
	}
	if (m_aAdjacentTriangles[i].Complete())
	{
	    m_nTrianglesWithCompleteAdjacency++;
	}
    }
}

int CMesh::FindAdjacentTriangle(int nTriangleID, int nSide)
{
    //int nP1 = m_aTriangles[nTriangleID]->Vertices()[nSide];
    //int nP2 = m_aTriangles[nTriangleID]->Vertices()[(nSide+1)%3];

    //for (int i = 0; i < m_aTriangles.size(); ++i)
    //{
    //	if (i == nTriangleID)
    //	{
    //		continue;
    //	}

    //	bool bHasP1 = false;
    //	bool bHasP2 = false;
    //	int nOther = -1;

    //	bHasP1 = (m_aTriangles[i]->Vertices().contains(nP1));
    //	bHasP2 = (m_aTriangles[i]->Vertices().contains(nP2));

    //	if (bHasP1 && bHasP2)
    //	{
    //		for (int j = 0; j < 3; ++j)
    //		{
    //			if (m_aTriangles[i]->Vertices()[j] != nP1
    //				&& m_aTriangles[i]->Vertices()[j] != nP2)
    //			{
    //				nOther = j;
    //			}
    //		}

    //		m_aAdjacentTriangles[nTriangleID].aAdjacentTriangles[nSide] = i;
    //		m_aAdjacentTriangles[nTriangleID].aAdjacentVertices[nSide] = m_aTriangles[i]->Vertices()[nOther];
    //		//	    m_aAdjacentTriangles[i].aAdjacentTriangles[nSide] = nTriangleID;
    //		//	    m_aAdjacentTriangles[i].aAdjacentVertices[nSide] = m_aTriangles[i].Vertices()[nOther];
    //		return i;
    //	}
    //	//	for (int j = 0; j < 3; ++j)
    //	//	{
    //	//	    bHasP1 = (m_aTriangles[i].Vertices()[j] == nP1);
    //	//	    bHasP2 = (m_aTriangles[i].Vertices()[(j+1)%3] == nP2);
    //	//	    if (bHasP1 && bHasP2)
    //	//	    {
    //	//		m_aAdjacentTriangles[i].aAdjacentTriangles[j] = nTriangleID;
    //	//		m_aAdjacentTriangles[i].aAdjacentVertices[j] = m_aTriangles[i].Vertices()[(j+2)%3];
    //	//		return i;
    //	//	    }
    //	//	}
    //}
    return -1;
}

void CMesh::BuildBezierTriangles()
{
    for (uint i = 0; i < m_aTriangles.size(); ++i)
    {
	m_aTriangles[i]->BuildBezierPoints();
	m_aTriangles[i]->Subdivide();
    }
    //    m_aBezierTriangles.reserve(m_nTrianglesWithCompleteAdjacency);
    //    for (int i = 0; i < m_aTriangles.size(); ++i)
    //    {
    //	if (m_aAdjacentTriangles[i].Complete())
    //	{
    //	    m_aBezierTriangles.push_back(new CBezierTriangle(i));
    //	}
    //    }
}

bool SAdjacencyOfTriangle::Complete(int nSide) const
{
    switch ( nSide )
    {
    case 0:
    {
	return Flags & k_AdjTriangles_Has_A;
    }
    case 1:
    {
	return Flags & k_AdjTriangles_Has_B;
    }
    case 2:
    {
	return Flags & k_AdjTriangles_Has_C;
    }
    default:
    {
	return Flags == k_AdjTriangles_Complete;
    }
    }
}

void SAdjacencyOfTriangle::SetComplete(int nSide)
{
    switch ( nSide )
    {
    case 0:
    {
	Flags |= k_AdjTriangles_Has_A;
	break;
    }
    case 1:
    {
	Flags |= k_AdjTriangles_Has_B;
	break;
    }
    case 2:
    {
	Flags |= k_AdjTriangles_Has_C;
	break;
    }
    }
}
