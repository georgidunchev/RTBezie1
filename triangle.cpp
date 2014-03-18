#include "triangle.h"
#include "intersactioninfo.h"
#include "ray.h"
#include "QVector2D"
#include "vector"
#include "qmath.h"
#include "QDebug"
#include "Utils.h"
#include "main.h"
#include "raytracer.h"
#include "cmesh.h"
#include "vector"
#include "settings.h"
#include "SubTriangle.h"
#include "color.h"


CTriangle::CTriangle()
    : m_aVertecis(GetRaytracer()->GetMesh().Vertices())
    , m_BezierPatch(*this)
{}

CTriangle::CTriangle( const std::vector<CVertex> &aVertecis, int v1, int v2, int v3, int nTriangleId)
    : m_aVertecis(GetRaytracer()->GetMesh().Vertices())
    , m_bHasBoundingBox(false)
    , m_nTriangleId(nTriangleId)
    , m_BezierPatch(*this)
    , m_aSubTriangles(GetSettings()->m_bHalfSubdvision ?
                      (CUtils::PowerOf2(GetSettings()->GetNofSubdivisions())) :
                      ((GetSettings()->GetNofSubdivisions() + 1)*(GetSettings()->GetNofSubdivisions() + 1)))
{
    m_aVertIndices.push_back(v1);
    m_aVertIndices.push_back(v2);
    m_aVertIndices.push_back(v3);

    m_vAB = B().GetPos() - A().GetPos();
    m_vAC = C().GetPos() - A().GetPos();
    m_vNormal = CVector3DF::Normal(AB(), AC());
}

CTriangle::~CTriangle()
{
    for (int i = 0, m = m_aSubTriangles.size(); i < m; ++i)
    {
        delete m_aSubTriangles[i];
    }
    m_aSubTriangles.clear();
}

const std::vector<int> &CTriangle::Vertices() const
{
    return m_aVertIndices;
}

CVertex& CTriangle::GetVertex(int i)
{
    return m_aVertecis[Vertices()[i]];
}

CVertex& CTriangle::GetVertex(int i) const
{
    return m_aVertecis[Vertices()[i]];
}
const CVector3DF &CTriangle::AB() const
{
    return m_vAB;
}

const CVector3DF &CTriangle::AC() const
{
    return m_vAC;
}

const CVector3DF &CTriangle::Normal() const
{
    return m_vNormal;
}

const CVertex &CTriangle::A() const
{
    return m_aVertecis[m_aVertIndices[0]];
}

const CVertex &CTriangle::B() const
{
    return m_aVertecis[m_aVertIndices[1]];
}

const CVertex &CTriangle::C() const
{
    return m_aVertecis[m_aVertIndices[2]];
}

void CTriangle::MakeBoundingBox()
{
    int nSize = m_aSubTriangles.size();

    for(int i = 0; i < nSize; ++i)
    {
        m_aSubTriangles[i]->MakeBoundingBox();
        m_BoundingBox.AddPoint(m_aSubTriangles[i]->GetBoundingBox().GetMinVertex());
        m_BoundingBox.AddPoint(m_aSubTriangles[i]->GetBoundingBox().GetMaxVertex());
    }
    //    qDebug()<<"min"<<m_BoundingBox.GetMinVertex()<<"max"<<m_BoundingBox.GetMaxVertex();
    m_bHasBoundingBox = true;
}

const CAABox& CTriangle::GetBoundingBox()
{
    return m_BoundingBox;
}

void CTriangle::BuildBezierPoints()
{
    m_BezierPatch.BuildBezierPoints();
}

void CTriangle::Subdivide()
{
    if(!GetSettings()->m_bHalfSubdvision)
    {
        //New test subdivision
        int nSubdivisionLevel = GetSettings()->GetNofSubdivisions();
//        m_aSubTriangles.resize((nSubdivisionLevel + 1)*(nSubdivisionLevel + 1));

        if (nSubdivisionLevel == 0)
        {
            m_aSubTriangles[0] = new CSubTriangle(*this);
            return;
        }

        const float fDenom = 1.0f / (float(nSubdivisionLevel + 1));

        for (int i = 0, k = 0; i <= nSubdivisionLevel; ++i)
        {
            for (int j = 0; j <= nSubdivisionLevel - i; ++j)
            {
                const float f100 = ((float)(nSubdivisionLevel + 1 - i - j)) * fDenom;
                const float f010 = ((float)j) * fDenom;
                const float f001 = ((float)i) * fDenom;
                const CVector3DF v100(f100, f010, f001);
                const CVector3DF v010(v100 + CVector3DF(-fDenom, fDenom, 0.0f));
                const CVector3DF v001(v100 + CVector3DF(-fDenom, 0.0f, fDenom));
                m_aSubTriangles[k] = new CSubTriangle(*this, v100, v010, v001);
                k++;
                if (i > 0)
                {
                    const CVector3DF v00_1(v100 + CVector3DF(0.0f, fDenom, -fDenom));
                    m_aSubTriangles[k] = new CSubTriangle(*this, v100, v010, v00_1);
                    k++;
                }
            }
        }
    }
    else
    {
//        m_aSubTriangles.resize(CUtils::PowerOf2(GetSettings()->GetNofSubdivisions()));

        m_aSubTriangles[0] = new CSubTriangle(*this);
        m_aSubTriangles[0]->Subdivide();

        int nSize = m_aSubTriangles.size();
        for (int i = 0; i < nSize; ++i)
        {
            m_aSubTriangles[i]->m_nSubtriangleID = i;
        }
    }
}

void CTriangle::AddSubTriangle(CSubTriangle* subTriangle)
{
    int nSavePos = subTriangle->GetSavePos();
    if (m_aSubTriangles[nSavePos])
    {
        delete m_aSubTriangles[nSavePos];
    }
    m_aSubTriangles[nSavePos] = subTriangle;
    m_aSubTriangles[nSavePos]->Subdivide();
}

int CTriangle::GetMemory() const
{
    const int size = m_aSubTriangles.size();
    int total = size==0?0: size * m_aSubTriangles.front()->GetMemory();
    total += m_BezierPatch.GetMemory();
    return total;
}

bool CTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
    QElapsedTimer timer;
    timer.start();

    const bool bIntersected = CSubTriangle::IntersectSubdevidedTriangles(ray, intersectionInfo, m_aSubTriangles, bDebug);

    intersectionInfo.m_nObjTime += timer.nsecsElapsed();
    intersectionInfo.m_nBezierIntersections += m_aSubTriangles.size();

    return bIntersected;
}

bool CTriangle::IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, std::vector<CVector3DF>* aPointsToCheck, bool bDebug) const
{
    return IntersectSubdevidedTriangles(ray, intersectionInfo, m_aSubTriangles, aPointsToCheck, bDebug);
}

/*static*/ bool CTriangle::IntersectSubdevidedTriangles(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<CSubTriangle*>& aSubTriangles, std::vector<CVector3DF>* aPointsToCheck, bool bDebug)
{
    int nSize = aSubTriangles.size();
    float fModifier = 8.0f / static_cast<float>(nSize);

    bool bIntersected = false;

    for (int i = 0; i < nSize; i++)
    {
        CIntersactionInfo intersectionInfoLocal(intersectionInfo);
        if( aSubTriangles[i]->Intersect(ray, intersectionInfoLocal,bDebug) )
        {
            int nId = aSubTriangles[i]->m_nSubtriangleID;

            if (false)
            {
                int nSubtriangleId = aSubTriangles[i]->m_nSubtriangleID;
                bool bR = ((nSubtriangleId / 4) > 0);
                bool bG = (((nSubtriangleId % 4) / 2) > 0);
                bool bB = (((nSubtriangleId % 4) % 2) > 0);

                float fR = bR ? fModifier * static_cast<float>(nSubtriangleId) * 0.125f : 0.0f;
                float fG = bG ? fModifier * static_cast<float>(nSubtriangleId) * 0.125f : 0.0f;
                float fB = bB ? fModifier * static_cast<float>(nSubtriangleId) * 0.125f : 0.0f;

                intersectionInfoLocal.color = CColor(fR, fG, fB);
            }

            intersectionInfoLocal.pSubTriangle = aSubTriangles[i];

            intersectionInfoLocal.m_nSubTriangleId = nId;

            if (intersectionInfoLocal.m_fDistance < intersectionInfo.m_fDistance)
            {
                intersectionInfo = intersectionInfoLocal;
                bIntersected = true;
            }
        }
    }
    return bIntersected;
}

bool CTriangle::Intersect(const CVector3DF &vStart, const CVector3DF &vEnd) const
{
    CIntersactionInfo Intersection;
    CRay Ray(vStart, vEnd);
    if (!Intersect(Ray, Intersection))
    {
        return false;
    }

    float fLength = (vEnd - vStart).Length();
    if (Intersection.m_fDistance < fLength)
    {
        return true;
    }
    return false;
}

