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
    , n_mLeafs(0)
    , m_bLoadedMesh(false)
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

//    GetMemoryKB();
}

void CMesh::ReadFromFile(const QString &strInputFileName)
{
    m_bLoadedMesh = false;

    QFile file(strInputFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream inputStream(&file);
    QString strLine;

    m_aVertices.clear();
    for (int i = 0, m = m_aTriangles.size(); i < m; ++i)
    {
        delete m_aTriangles[i];
    }
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

    m_bLoadedMesh = !m_aTriangles.empty();
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
    if (GetSettings()->UseKDTree())
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
//        int i = 0;
//        CVector3DF vIntersection = intersectionInfo.m_vIntersectionPoint;
//        CVector3DF vLIghtDirection = GetRaytracer()->GetLightScene().GetLight(i).GetPosition() - vIntersection;
//        vLIghtDirection.Normalize();
//        vIntersection += vLIghtDirection * k_fSMALL;

//        //CRay LightRay(GetLightScene().GetLight(i).GetPosition(), vLIghtDirection);
//        CRay LightRay(vIntersection, vLIghtDirection);

//        float fDistToLight = vLIghtDirection.Length();

//        CIntersactionInfo LightIntersectionInfo;
//        LightIntersectionInfo.m_bInitialRay = false;
//        if (Intersect(LightRay, LightIntersectionInfo))
//        {
//            //if the first intersection is the intersection point
//            if (LightIntersectionInfo.m_fDistance < fDistToLight - k_fSMALL)
//            {
//                CColor colorForLight(0.2f, 0.2f, 0.2f);
//                intersectionInfo.color = colorForLight;
//            }
//        }
//    }

    return bResult;
}

bool CMesh::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<int>& aTriangles, CAABox* pBBox, bool bDebug)
{
    bool bIntersected = false;
    for (uint i = 0; i < aTriangles.size(); ++i)
    {
        CTriangle* Triangle = GetPrimitive(aTriangles[i]);

        if ( Triangle->Intersect(ray, intersectionInfo, bDebug) )
        {
            bIntersected = true;
        }
    }
    return bIntersected;
}

bool CMesh::IntersectKDTree(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug)
{
    return m_pRoot->Intersect(ray, intersectionInfo, bDebug);
}

int CMesh::GetMemorySubKB() const
{
    int size = m_aTriangles.size();
    const int subtrianglesSize = size==0?0:(size * m_aTriangles.front()->GetMemory())/1024;
    return subtrianglesSize;
}

int CMesh::GetMemoryKDtreeKB() const
{
    const int kdtreeSize = m_pRoot->GetMemory()/1024;
    return kdtreeSize;
}

bool CMesh::isMeshLoaded() const
{
    return m_bLoadedMesh;
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

        m_nNOfSubtriangles = nNumberOfSubTriangles * nNumberOfPrimitives;

        pAllSubTriangles->reserve(m_nNOfSubtriangles);

        for (int j = 0; j < nNumberOfPrimitives; j++)
        {
            for (int i = 0; i < nNumberOfSubTriangles; ++i)
            {
                pAllSubTriangles->push_back(GetPrimitive(j)->GetSubTriangle(i));
            }
        }
    }

    if (m_pRoot)
    {
        delete m_pRoot;
    }
    m_pRoot = new CKDTreeNode(pAllSubTriangles, 0, m_BoundingBox);
    n_mLeafs = m_pRoot->Process();
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
