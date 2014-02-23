#include <qalgorithms.h>
#include "cmesh.h"
#include "QTextStream"
#include <QDebug>
#include <QFile>
#include "kdtreenode.h"
#include "AABox.h"
#include "main.h"
#include "raytracer.h"
#include "settings.h"
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

            m_aVertices.push_back(CVertex(CVector3DF(aCoords[0], aCoords[1], aCoords[2]), m_aVertices.size()));
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
    intersectionInfo.m_bHighQuality = GetRaytracer()->IsHighQuality();

    bool bResult = false;
    if (GetSettings()->m_bUseKDTtee)
    {
        bResult = IntersectKDTree(ray, intersectionInfo, bDebug);
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

    //    if(bResult && intersectionInfo.m_bInitialRay)
    //    {
    //	int i = 0;
    //	QVector3D vIntersection = intersectionInfo.m_vIntersectionPoint;
    //	QVector3D vLIghtDirection = GetRaytracer()->GetLightScene().GetLight(i).GetPosition() - vIntersection;
    //	vLIghtDirection.normalize();
    //	vIntersection += vLIghtDirection * k_fSMALL;

    //	//CRay LightRay(GetLightScene().GetLight(i).GetPosition(), vLIghtDirection);
    //	CRay LightRay(vIntersection, vLIghtDirection);

    //	float fDistToLight = vLIghtDirection.length();

    //	CIntersactionInfo LightIntersectionInfo;
    //	LightIntersectionInfo.m_bInitialRay = false;
    //	if (Intersect(LightRay, LightIntersectionInfo))
    //	{
    //	    //if the first intersection is the intersection point
    //	    if (LightIntersectionInfo.m_fDistance < fDistToLight - k_fSMALL)
    //	    {
    //		CColor colorForLight(0.f, 0.f, 0.f);
    //		intersectionInfo.color = colorForLight;
    //	    }
    //	}
    //    }

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
                CVector3DF vIntersection = ray.GetPointAtDistance(LastIntersection.m_fDistance);
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
    return &m_aTriangles;
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

void CMesh::GenerateKDTree()
{
    std::vector<CSubTriangle*>* pAllSubTriangles = new std::vector<CSubTriangle*>;

    int nNumberOfPrimitives = GetPrimitives()->size();

    if (nNumberOfPrimitives > 0)
    {
        int nNumberOfSubTriangles = GetPrimitive(0)->GetNumberOfSubTriangle();

        pAllSubTriangles->reserve(nNumberOfSubTriangles * nNumberOfPrimitives);

        for (int j = 0; j < nNumberOfPrimitives; j++)
        {
            for (int i = 0; i < nNumberOfSubTriangles; ++i)
            {
                pAllSubTriangles->push_back(GetPrimitive(j)->GetSubTriangle(i));
            }
        }
    }

    CUtils::SafeDel(m_pRoot);
    m_pRoot = new CKDTreeNode(pAllSubTriangles, 0, m_BoundingBox);
    m_pRoot->Process();
}

int CMesh::GetKDTreeNextID()
{
    return 1;
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
