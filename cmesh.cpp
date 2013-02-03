#include "cmesh.h"
#include "QTextStream"
#include <QDebug>
#include <QFile>
#include "kdtreenode.h"

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

void CMesh::GenerateKDTree()
{
    //initialize an array containing all triangles
    QVector<int>* pAllTriangleIndeces = new QVector<int>(m_aTriangles.size());
    for (int i = 0; i < m_aTriangles.size(); ++i)
    {
	(*pAllTriangleIndeces)[i] = i;
    }

    m_pRoot = new CKDTreeNode();
}
