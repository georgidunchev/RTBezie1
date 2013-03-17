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

CMesh::CMesh(QObject *parent)
    : QObject(parent)
{

}

void CMesh::Load(const QString& strInputFileName)
{
    QFile file(strInputFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
	return;
    }

    QString strReturnMessage("Data loaded correctly");
    QTextStream inputStream(&file);
    QString strLine;

    m_aVertices.append(QVector3D(0.f, 0.f, 0.f));

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
	    QVector<qreal> temp;
	    for (int i = 0; i < 3; ++i)
	    {
		lineStream >> strMarker;
		if(strMarker.isEmpty())
		{
		    //strReturnMessage << "Error: Number of Elements in Vertex: " << m_aVertices.size();
		    break;
		}

		temp.append(strMarker.toDouble());
	    }
	    m_aVertices.append(QVector3D(temp[0], temp[1], temp[2]));
	}
	else if (strMarker == "f" )
	{
	    QVector<int> temp;
	    lineStream >> strMarker;
	    int i = 0;

	    while(!strMarker.isEmpty())
	    {
		i++;
		temp.append(strMarker.toInt());
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

	    for ( int i = 2; i < temp.size(); ++i)
	    {
		m_aTriangles.append(new CTriangle(m_aVertices, temp[0], temp[i-1], temp[i]));
	    }
	}
    }
    file.close();

    mBoundingBox.Reset();
    for (int i = 0; i < m_aTriangles.size(); ++i)
    {
	m_aTriangles[i]->MakeBoundingBox();
	mBoundingBox.AddPoint(m_aTriangles[i]->GetBoundingBox().GetMinVertex());
	mBoundingBox.AddPoint(m_aTriangles[i]->GetBoundingBox().GetMaxVertex());
    }

    GenerateKDTree();
    m_nTrianglesWithCompleteAdjacency = 0;
    BuildAdjacency();
    BuildBezierTriangles();
}

bool CMesh::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    if (k_bUSE_KDTREE)
    {
	return IntersectKDTree(ray, intersectionInfo);
    }
    else
    {
	QVector<int> aTriangles(GetPrimitives()->size());
	for (int i = 0; i < aTriangles.size(); ++i)
	{
	    aTriangles[i] = i;
	}
	return Intersect(ray, intersectionInfo, aTriangles);
    }
}

bool CMesh::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, const QVector<int>& aTriangles, CAABox* pBBox)
{
    bool bIntersected = false;

    for (int i = 0; i < aTriangles.size(); ++i)
    {
	CPrimitive* Triangle = GetPrimitive(aTriangles[i]);
	CIntersactionInfo LastIntersection;
	if ( Triangle->Intersect(ray, LastIntersection) )
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

bool CMesh::IntersectKDTree(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    return m_pRoot->Intersect(ray, intersectionInfo);
}

QVector<CPrimitive*> *CMesh::GetPrimitives()
{
    if (GetSettings()->GetIntersectBezier())
    {
	return &m_aBezierTriangles;
    }
    else
    {
	return &m_aTriangles;
    }
}

QVector<CPrimitive *> &CMesh::GetTriangles()
{
    return m_aTriangles;
}

CPrimitive *CMesh::GetPrimitive(int n)
{
    QVector<CPrimitive*>& tmpVector = *GetPrimitives();
    return tmpVector[n];
}

const QVector<QVector3D> &CMesh::GetVertices()
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
    QVector<int>* pAllTriangleIndeces = new QVector<int>(GetPrimitives()->size());
    for (int i = 0; i < pAllTriangleIndeces->size(); ++i)
    {
	(*pAllTriangleIndeces)[i] = i;
    }

    m_pRoot = new CKDTreeNode(pAllTriangleIndeces, 0, mBoundingBox);
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
	m_aSortedBBoxes[i].resize(nSize);
	for (int j = 0; j < m_aSortedBBoxes[i].size(); ++j)
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

void CMesh::BuildAdjacency()
{
    m_aAdjacentTriangles.resize(m_aTriangles.size());
    for (int i = 0; i < m_aAdjacentTriangles.size(); ++i)
    {
	for (int j = 0; j < 3; ++j)
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
    int nP1 = m_aTriangles[nTriangleID]->Vertices()[nSide];
    int nP2 = m_aTriangles[nTriangleID]->Vertices()[(nSide+1)%3];

    for (int i = 0; i < m_aTriangles.size(); ++i)
    {
	if (i == nTriangleID)
	{
	    continue;
	}

	bool bHasP1 = false;
	bool bHasP2 = false;
	int nOther = -1;

	bHasP1 = (m_aTriangles[i]->Vertices().contains(nP1));
	bHasP2 = (m_aTriangles[i]->Vertices().contains(nP2));

	if (bHasP1 && bHasP2)
	{
	    for (int j = 0; j < 3; ++j)
	    {
		if (m_aTriangles[i]->Vertices()[j] != nP1
			&& m_aTriangles[i]->Vertices()[j] != nP2)
		{
		    nOther = j;
		}
	    }

	    m_aAdjacentTriangles[nTriangleID].aAdjacentTriangles[nSide] = i;
	    m_aAdjacentTriangles[nTriangleID].aAdjacentVertices[nSide] = m_aTriangles[i]->Vertices()[nOther];
	    //	    m_aAdjacentTriangles[i].aAdjacentTriangles[nSide] = nTriangleID;
	    //	    m_aAdjacentTriangles[i].aAdjacentVertices[nSide] = m_aTriangles[i].Vertices()[nOther];
	    return i;
	}
	//	for (int j = 0; j < 3; ++j)
	//	{
	//	    bHasP1 = (m_aTriangles[i].Vertices()[j] == nP1);
	//	    bHasP2 = (m_aTriangles[i].Vertices()[(j+1)%3] == nP2);
	//	    if (bHasP1 && bHasP2)
	//	    {
	//		m_aAdjacentTriangles[i].aAdjacentTriangles[j] = nTriangleID;
	//		m_aAdjacentTriangles[i].aAdjacentVertices[j] = m_aTriangles[i].Vertices()[(j+2)%3];
	//		return i;
	//	    }
	//	}
    }
    return -1;
}

void CMesh::BuildBezierTriangles()
{
    m_aBezierTriangles.reserve(m_nTrianglesWithCompleteAdjacency);
    for (int i = 0; i < m_aTriangles.size(); ++i)
    {
	if (m_aAdjacentTriangles[i].Complete())
	{
	    m_aBezierTriangles.append(new CBezierTriangle(i));
	}
    }
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
