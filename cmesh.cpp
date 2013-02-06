#include <qalgorithms.h>
#include "cmesh.h"
#include "QTextStream"
#include <QDebug>
#include <QFile>
#include "kdtreenode.h"
#include "AABox.h"

CMesh::CMesh(QObject *parent)
    : QObject(parent)
{

}

void CMesh::Load(const QString& strInputFileName)
{
    QFile file(strInputFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	return;

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
		m_aTriangles.append(CTriangle(m_aVertices, temp[0], temp[i-1], temp[i]));
	    }
	}
    }
    file.close();

    for (int i = 0; i < m_aTriangles.size(); ++i)
    {
	m_aTriangles[i].MakeBoundingBox();
    }
}

bool CMesh::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo)
{
    bool bIntersected = false;

    for (int i = 0; i < m_aTriangles.size(); ++i)
    {
	CIntersactionInfo LastIntersection;
	if ( m_aTriangles[i].Intersect(ray, LastIntersection) )
	{
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

CTriangle &CMesh::GetTriangle(int n)
{
    return m_aTriangles[n];
}

void CMesh::GenerateKDTree()
{
    SortBBoxes();
    //initialize an array containing all triangles
    QVector<int>* pAllTriangleIndeces = new QVector<int>(m_aTriangles.size());
    for (int i = 0; i < m_aTriangles.size(); ++i)
    {
	(*pAllTriangleIndeces)[i] = i;
    }

    m_pRoot = new CKDTreeNode();
}

bool CMesh::CompareBB(const CSortedBBEntry &s1, const CSortedBBEntry &s2)
{
    const CAABox& BB1 = m_aTriangles[s1.m_nTriangleId].GetBoundingBox();
    const QVector3D& vBB1( s1.m_bStart ? BB1.GetMinVertex() : BB1.GetMaxVertex() );
    float fValue1 = CUtils::GetDimension(vBB1, m_eSortingDimention);

    const CAABox& BB2 = m_aTriangles[s2.m_nTriangleId].GetBoundingBox();
    const QVector3D& vBB2( s2.m_bStart ? BB2.GetMinVertex() : BB2.GetMaxVertex() );
    float fValue2 = CUtils::GetDimension(vBB2, m_eSortingDimention);

    return fValue1 < fValue2;
}

void CMesh::SortBBoxes()
{
    CSortedBBEntry a;
    a.m_pMesh = this;

    int nSize = m_aTriangles.size() * 2;
    for(EDimiensions i = e_Dimension_X; i < e_Dimension_MAX; i = (EDimiensions)((int)i + 1) )
    {
	a.m_eSortingDimention = i;

	m_aSortedBBoxes[i].resize(nSize);
	for (int j = 0; j < m_aSortedBBoxes[i].size(); ++j)
	{
	    m_aSortedBBoxes[i][j].m_bStart = j%2==0;
	    m_aSortedBBoxes[i][j].m_nTriangleId = j/2;
	}


	qSort(m_aSortedBBoxes[i].begin(), m_aSortedBBoxes[i].end());
//	qSort(m_aSortedBBoxes[i].begin(), m_aSortedBBoxes[i].end(), CompareBB);

//	const float fMinVDir( CUtils::GetDimension(m_vMinVertex, i) );
//	const float fMaxVDir( CUtils::GetDimension(m_vMaxVertex, i) );
//	const float fPointDir( CUtils::GetDimension(vPoint, i) );

//	if(fMinVDir > fPointDir)
//	{
//	    CUtils::SetDimension(m_vMinVertex, i, fPointDir);
//	}

//	if(fMaxVDir < fPointDir)
//	{
//	    CUtils::SetDimension(m_vMaxVertex, i, fPointDir);
//	}
    }
    //    m_aSortedBBoxes
}

bool CSortedBBEntry::operator <(const CSortedBBEntry &s2) const
{
    const CAABox& BB1 = m_pMesh->GetTriangle(m_nTriangleId).GetBoundingBox();
    const QVector3D& vBB1( m_bStart ? BB1.GetMinVertex() : BB1.GetMaxVertex() );
    float fValue1 = CUtils::GetDimension(vBB1, m_eSortingDimention);

    const CAABox& BB2 = s2.m_pMesh->GetTriangle(m_nTriangleId).GetBoundingBox();
    const QVector3D& vBB2( s2.m_bStart ? BB2.GetMinVertex() : BB2.GetMaxVertex() );
    float fValue2 = CUtils::GetDimension(vBB2, m_eSortingDimention);

    return fValue1 < fValue2;
}
