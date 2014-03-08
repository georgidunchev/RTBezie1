#include "bezierpatch.h"
#include "intersactioninfo.h"
#include "ray.h"
#include "Utils.h"
#include "qmath.h"
#include "QDebug"
#include "triangle.h"
#include "SubTriangle.h"
#include "settings.h"
#include "main.h"

CBezierPatch::CBezierPatch(CTriangle& pParent)
    : m_Parent_Triangle(pParent)
    , m_pParent_SubTriangle(NULL)
    , m_aAdditionalPoints(10, CVector3DF(0, 0, 0))
{
}

CBezierPatch::CBezierPatch(CSubTriangle *pParent)
    : m_Parent_Triangle(pParent->GetParent())
    , m_pParent_SubTriangle(pParent)
    , m_aAdditionalPoints(10, CVector3DF(0, 0, 0))
{
}

const CVector3DF CBezierPatch::GetPointFromBarycentric(const CVector3DF& vCoords) const
{
    return GetPointFromBarycentric(vCoords.X(), vCoords.Y());
}

const CVector3DF CBezierPatch::GetPointFromBarycentric(const QVector2D& vCoords) const
{
    return GetPointFromBarycentric(vCoords.x(), vCoords.y());
}

const CVector3DF CBezierPatch::GetPointFromBarycentric(float u, float v) const
{
    CVector3DF B = Q30 * u*u*u
            + Q03 * v*v*v
            + Q21 * u*u*v
            + Q12 * u*v*v
            + Q20 * u*u
            + Q02 * v*v
            + Q11 * u*v
            + Q10 * u
            + Q01 * v
            + Q00;
    return B;
}

const CVector3DF CBezierPatch::GetdBu(float u, float v) const
{
    //partial U derivative of B
    CVector3DF dBu = 3 * Q30 * u * u
            + 2 * Q21 * u * v
            + Q12 * v * v
            + 2 * Q20 * u
            + Q11 * v
            + Q10;
    return dBu;
}

const CVector3DF CBezierPatch::GetdBv(float u, float v) const
{
    //Partial V derivative of B
    CVector3DF dBv = 3 * Q03 * v * v
            + 2 * Q12 * u * v
            + Q21 * u * u
            + 2 * Q02 * v
            + Q11 * u
            + Q01;
    return dBv;
}

const CVector3DF CBezierPatch::GetSmoothedNormal(const CVector3DF &vCoords) const
{
    const CVector3DF vNormalA = vCoords.X() * m_Parent_Triangle.A().Normal_Get();
    const CVector3DF vNormalB = vCoords.Y() * m_Parent_Triangle.B().Normal_Get();
    const CVector3DF vNormalC = vCoords.Z() * m_Parent_Triangle.C().Normal_Get();
    return (vNormalA + vNormalB + vNormalC).Normalized();
}

const CVector3DF CBezierPatch::GetSubSurfSmoothedNormal(const CVector3DF &vCoords) const
{
    const CVector3DF vLocalCoords ;//= GetSubBar(vCoords);
    const CVector3DF vNormalA = vLocalCoords.X() * CVector3DF::Normal(GetdBu(1.0f,0.0f), GetdBv(1.0f,0.0f));
    const CVector3DF vNormalB = vLocalCoords.Y() * CVector3DF::Normal(GetdBu(0.0f,1.0f), GetdBv(0.0f,1.0f));
    const CVector3DF vNormalC = vLocalCoords.Z() * CVector3DF::Normal(GetdBu(0.0f,0.0f), GetdBv(0.0f,0.0f));

    return (vNormalA + vNormalB + vNormalC).Normalized();
}

void CBezierPatch::BuildBezierPoints()
{
    m_aAdditionalPoints[0] = m_Parent_Triangle.A().GetPos();
    m_aAdditionalPoints[1] = m_Parent_Triangle.B().GetPos();
    m_aAdditionalPoints[2] = m_Parent_Triangle.C().GetPos();

    std::vector<CVertexInfo> aVertices;
    // Push the needed information for generating the additional points
    aVertices.push_back(CVertexInfo(Point(3,0),
                                    Point(0,3), Point(2,1),
                                    Point(0,0), Point(2,0),
                                    m_Parent_Triangle.A().Normal_Get()));

    aVertices.push_back(CVertexInfo(Point(0,3),
                                    Point(0,0), Point(0,2),
                                    Point(3,0), Point(1,2),
                                    m_Parent_Triangle.B().Normal_Get()));

    aVertices.push_back(CVertexInfo(Point(0,0),
                                    Point(3,0), Point(1,0),
                                    Point(0,3), Point(0,1),
                                    m_Parent_Triangle.C().Normal_Get()));

    //Generate the new bezier points
    unsigned int nSize = aVertices.size();
    for( unsigned int i = 0; i < nSize; ++i )
    {
        BuildBezierPoint(aVertices[i]);
    }

    //Generate last point
    CUtils::TriangleCentre( Point(1,1),
                            Point(2,0), Point(1,2), Point(0,1));

    //    CVector3DF vE = Point(2,1) + Point(1,2) + Point(0,2) + Point(0,1) + Point(1,0) + Point(2,0);
    //    vE /= 6;
    //    CVector3DF vV = Point(0,0) + Point(3,0) + Point(0,3);

    //    Point(1,1) = vE + (vE - vV)*0.5f;

    BuildBezierPoints_Internal();
}

void CBezierPatch::BuildBezierPoints_InitialSub()
{
    for (uint i = 0; i < 10; ++i)
    {
        Point(i) = m_pParent_SubTriangle->GetParent().GetBezierPatch().Point(i);
    }
    BuildBezierPoints_Internal();
}

void CBezierPatch::BuildBezierPoints_Sub(int nStartOfLongest, bool bFirst)
{
    int a[3], b[3];
    switch (nStartOfLongest)
    {
    case 0:
    {
        a[0] = a[1] = a[2] = 3;
        b[0] = b[1] = b[2] = 0;
        break;
    }
    case 1:
    {
        a[0] = a[1] = a[2] = 0;
        b[0] = b[1] = b[2] = 3;
        break;
    }
    case 2:
    {
        a[0] = a[1] = a[2] = 0;
        b[0] = b[1] = b[2] = 0;
        break;
    }
    }

    int pos1 = (2 + nStartOfLongest)%3;

    CUtils::GetNextPoint(a[1], b[1], nStartOfLongest, -3);
    CUtils::GetNextPoint(a[2], b[2], pos1, 3);

    CBezierPatch& ParentPatch = *m_pParent_SubTriangle->GetParentSubTriangle()->GetBezierPatch();

    //003
    Point(a[2], b[2]) = ParentPatch.GetPoint(a[2],b[2]);
    //    qDebug() << a[2] << b[2] << (bFirst?"Left":"Right");

    std::vector<int> c;
    std::vector<int> d;
    CVector3DF t[4];
    //102 &012
    c.assign(2, a[2]);
    d.assign(2, b[2]);
    CUtils::GetNextPoint(c[0], d[0], (2 + nStartOfLongest)%3, -1);
    CUtils::GetNextPoint(c[1], d[1], (1 + nStartOfLongest)%3, 1);

    if (!bFirst)
    {
        Reverse(c, d);
    }

    //    qDebug() << c[0] << d[0] << "," << c[1] << d[1];

    t[0] = ParentPatch.GetPoint(c[0],d[0]);
    t[1] = ParentPatch.GetPoint(c[1],d[1]);
    Point(c[0], d[0]) = t[0];
    Point(c[1], d[1]) = (t[0] + t[1]) * 0.5f;

    //201 & 111 & 021
    c.clear();
    d.clear();
    c.assign(3, a[2]);
    d.assign(3, b[2]);
    CUtils::GetNextPoint(c[0], d[0], (2 + nStartOfLongest)%3, -2);
    c[1] = 1; d[1] = 1;
    CUtils::GetNextPoint(c[2], d[2], (1 + nStartOfLongest)%3, 2);

    if (!bFirst)
    {
        Reverse(c, d);
    }

    //    qDebug() << c[0] << d[0] << "," << c[1] << d[1] << "," << c[2] << d[2];

    t[0] = ParentPatch.GetPoint(c[0],d[0]);
    t[1] = ParentPatch.GetPoint(c[1],d[1]);
    t[2] = ParentPatch.GetPoint(c[2],d[2]);
    Point(c[0], d[0]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    t[1] = (t[1]+t[2]) * 0.5f;
    Point(c[1], d[1]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    Point(c[2], d[2]) = t[0];

    //030 & 210 & 120 & 030
    c.clear();
    d.clear();
    c.assign(4, a[0]);
    d.assign(4, b[0]);
    CUtils::GetNextPoint(c[1], d[1], nStartOfLongest, -1);
    CUtils::GetNextPoint(c[2], d[2], nStartOfLongest, -2);
    c[3] = a[1]; d[3]= b[1];

    if (!bFirst)
    {
        Reverse(c, d);
    }

    //    qDebug() << c[0] << d[0] << "," << c[1] << d[1] << "," << c[2] << d[2] << "," << c[3] << d[3];

    t[0] = ParentPatch.GetPoint(c[0],d[0]);
    t[1] = ParentPatch.GetPoint(c[1],d[1]);
    t[2] = ParentPatch.GetPoint(c[2],d[2]);
    t[3] = ParentPatch.GetPoint(c[3],d[3]);

    Point(c[0], d[0]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    t[1] = (t[1]+t[2]) * 0.5f;
    t[2] = (t[2] + ParentPatch.GetPoint(c[3],d[3])) * 0.5f;
    Point(c[1], d[1]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    t[1] = (t[1]+t[2]) * 0.5f;
    Point(c[2], d[2]) = t[0];
    t[0] = (t[0]+t[1]) * 0.5f;
    Point(c[3], d[3]) = t[0];
    BuildBezierPoints_Internal();
}

void CBezierPatch::BuildBezierPoints_Sub2()
{
//    return;
//    int nSubdivisionLevel = GetSettings()->GetNofSubdivisions();
    int nN = 3;
    for (int l = nN; l >= 0; --l)
    {
        for (int m = 0; m <= l; ++m)
        {
            int u = l-m;
            int v = m;
            int w = nN - l;
            Point(u, v) = CalculateBezierPoint(u, v, w);
        }
    }
    BuildBezierPoints_Internal();
}

CVector3DF CBezierPatch::CalculateBezierPoint(int u, int v, int w) const
{
    CVector3DF aPoints[10];
    int nN = 3;
    for (int i = nN, k = 0; i >= 0; --i)
    {
        for (int j = 0; j <= i; ++j)
        {
            aPoints[k] = m_Parent_Triangle.GetBezierPatch().GetPoint(i-j, j);
            k++;
        }
    }

    const CVector3DF vU = m_pParent_SubTriangle->GetVertBar(0);
    const CVector3DF vV = m_pParent_SubTriangle->GetVertBar(1);
    const CVector3DF vW = m_pParent_SubTriangle->GetVertBar(2);

    for (int k = nN; k > 0; --k)
    {
        const CVector3DF* vUsed;
        if (u > 0)
        {
            vUsed = &vU;
            u--;
        }
        else if (v > 0)
        {
            vUsed = &vV;
            v--;
        }
        else
        {
            vUsed = &vW;
            w--;
        }

        int nC = 0;
        int nB = 0;
        for (int j = k; j > 0; --j)
        {
            for (int i = 0; i < j; ++i)
            {
                int a = nB;
                int b = nB+1;
                int c = nB + j + 1;
                aPoints[nC] = aPoints[a] * vUsed->X()
                            + aPoints[b] * vUsed->Y()
                            + aPoints[c] * vUsed->Z();
                nC++;
                nB++;
            }
            nB++;
        }
    }

    return aPoints[0];
}

void CBezierPatch::BuildBezierPoints_Internal()
{
    Q30 = Point(3,0) - Point(0,0) - 3 * Point(2,0) + 3 * Point(1,0);
    Q03 = Point(0,3) - Point(0,0) - 3 * Point(0,2) + 3 * Point(0,1);
    Q21 = 3 * Point(2,1) - 3 * Point(0,0) - 3 * Point(2,0) + 6 * Point(1,0) + 3 * Point(0,1) - 6 * Point(1,1);
    Q12 = 3 * Point(1,2) - 3 * Point(0,0) - 3 * Point(0,2) + 3 * Point(1,0) + 6 * Point(0,1) - 6 * Point(1,1);
    Q20 = 3*Point(0,0) + 3*Point(2,0) - 6*Point(1,0);
    Q02 = 3*Point(0,0) + 3*Point(0,2) - 6*Point(0,1);
    Q11 = 6*Point(0,0) - 6*Point(1,0) - 6*Point(0,1) + 6*Point(1,1);
    Q10 = 3*Point(1,0) - 3*Point(0,0);
    Q01 = 3*Point(0,1) - 3*Point(0,0);
    Q00 = Point(0,0);
}

void CBezierPatch::BuildBezierPoint(CVertexInfo &o_Info)
{
    BuildBezierPoint(*o_Info.vNew1, *o_Info.vMain, *o_Info.vEnd1, *o_Info.vNormal);
    BuildBezierPoint(*o_Info.vNew2, *o_Info.vMain, *o_Info.vEnd2, *o_Info.vNormal);
}

void CBezierPatch::BuildBezierPoint(CVector3DF &o_vNew,
                                    const CVector3DF &i_vMain,
                                    const CVector3DF &i_vEnd,
                                    const CVector3DF &i_vNormal)
{
    o_vNew = i_vEnd - i_vMain;
    o_vNew *= k_fOneThird;
    o_vNew = o_vNew.ProjectionInPlane(i_vNormal);
    o_vNew += i_vMain;
}

void CBezierPatch::Reverse(std::vector<int> &io_c, std::vector<int> &io_d)
{
    for (uint i = 0, n = io_c.size(); i < n/2; ++i)
    {
        const int j = n-1-i;
        std::swap(io_c[i], io_c[j]);
        std::swap(io_d[i], io_d[j]);
    }
}

int CBezierPatch::GetIndex(int a, int b) const
{
    if( a == 3 && b == 0 )
    {
        return 0;
    }
    else if( a == 0 && b == 3 )
    {
        return 1;
    }
    else if( a == 0 && b == 0 )
    {
        return 2;
    }
    else if( a == 2 && b == 1 )
    {
        return 3;
    }
    else if( a == 2 && b == 0 )
    {
        return 4;
    }
    else if( a == 0 && b == 2 )
    {
        return 5;
    }
    else if( a == 1 && b == 2 )
    {
        return 6;
    }
    else if( a == 1 && b == 0 )
    {
        return 7;
    }
    else if( a == 0 && b == 1 )
    {
        return 8;
    }
    else if( a == 1 && b == 1 )
    {
        return 9;
    }
    else
    {
        Q_ASSERT(false);
    }
}

CVector3DF& CBezierPatch::Point(int a, int b)
{
    return m_aAdditionalPoints[GetIndex(a,b)];
}

CVector3DF &CBezierPatch::Point(int idx)
{
    return m_aAdditionalPoints[idx];
}

const CVector3DF &CBezierPatch::GetPoint(int a, int b) const
{
    return m_aAdditionalPoints[GetIndex(a,b)];
}

const CVector3DF &CBezierPatch::GetPoint(QVector2D bar) const
{
    return m_aAdditionalPoints[GetIndex(bar.x(),bar.y())];
}

bool CBezierPatch::IntersectHighQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
    //    return IntersectLowQuality(ray, intersectionInfo, bDebug);

    //    return false;
    std::vector<CVector3DF> aPointsToCheck;
    //    CSubTriangle& SubTriangle = * intersectionInfo.pSubTriangle;

    //    aPointsToCheck.push_back(CVector3DF(intersectionInfo.u,intersectionInfo.v,intersectionInfo.w));
    //    for (int j = 0; j < 3; j++)
    //    {
    //	aPointsToCheck.push_back(SubTriangle.GetVertBar(j));
    //    }

    //    aPointsToCheck.resize(10);
    //    int u = 0, v = 0;
    //    uint k = 0;
    //    for (uint i = 0; i < 4; ++i)
    //    {
    //	if (i > 0)
    //	{
    //	    CUtils::GetNextPoint(u, v, 2, -1);
    //	}

    //	aPointsToCheck[k] = CVector3DF(u, v, 3 - u - v);
    //	aPointsToCheck[k] /= 3.0f;
    //	k++;

    //	int u1 = u, v1 = v;
    //	for (uint j = 0; j < i; ++j)
    //	{
    //	    CUtils::GetNextPoint(u1, v1, 0, -1);
    //	    aPointsToCheck[k] = CVector3DF(u1, v1, 3 - u1 - v1);
    //	    aPointsToCheck[k] /= 3.0f;
    //	    k++;
    //	}
    //    }
    float fA = 0.33f;
    aPointsToCheck.push_back( CVector3DF(fA, fA, 1.0f - 2 * fA) );
    if (GetSettings()->m_bMultipleSeeds)
    {
        aPointsToCheck.push_back( CVector3DF(1.0f, 0.0f, 0.0f) );
        aPointsToCheck.push_back( CVector3DF(0.0f, 1.0f, 0.0f) );
        aPointsToCheck.push_back( CVector3DF(0.0f, 0.0f, 1.0f) );
    }

    //    for (uint i = 0, n = m_aAdditionalPoints.size(); i < n; ++i)
    //    {
    //	aPointsToCheck.push_back(GetPointFromBarycentric(m_aAdditionalPoints[i]));
    //    }

    //    float fU = intersectionInfo.u;
    //    float fV = intersectionInfo.v;

    float closestdist = k_fMAX;
    intersectionInfo.m_fDistance = k_fMAX;

    const int nIterations = GetSettings()->m_nIterations;
    bool bezierFast = false;
    if (!bezierFast)
    {
        for (int i = 0, n = aPointsToCheck.size(); i < n; ++i)
        {
            //	    if (bDebug)
            //	    {
            //		qDebug() <<"Bar"<<aPointsToCheck[i];
            //		qDebug() <<"Point "<< GetPointFromBarycentric(aPointsToCheck[i]);
            //	    }

            CIntersactionInfo LocalInfo(intersectionInfo);
            if (intersect(ray, LocalInfo, aPointsToCheck[i], nIterations, bDebug))
            {
                intersectionInfo = LocalInfo;
                //intersectionInfo.color = CColor(1.0f- aPointsToCheck[i].x(), 1.0f - aPointsToCheck[i].y(), 1.0f - aPointsToCheck[i].z());
                closestdist = intersectionInfo.m_fDistance;
                return true;
            }
        }
        return false;
    }
    //    else
    //    {
    //	int iterations = 5;
    //	CVector3DF res = CVector3DF(1.0/3.0, 1.0/3.0, 0);
    //	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
    //	res = CVector3DF(1.0/6.0, 1.0/6.0, 0);
    //	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
    //	res = CVector3DF(2.0/3.0, 1.0/6.0, 0);
    //	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );
    //	res = CVector3DF(1.0/6.0, 2.0/3.0 ,0);
    //	intersectSimpleBezierTriangle(ray, intersectionInfo, SubTriangle, res, iterations, bDebug );

    //	closestdist = intersectionInfo.m_fDistance;
    //    }

    return fabs(closestdist - k_fMAX) > k_fMIN;
}

bool CBezierPatch::IntersectHighQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, const CSubTriangle& SubTriangle, bool bDebug) const
{
    CVector3DF aPointsToCheck[4];

    aPointsToCheck[0] = CVector3DF(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; ++i)
    {
        aPointsToCheck[i+1] = SubTriangle.GetVertBar(i);
        aPointsToCheck[0] += aPointsToCheck[i+1];
    }
    aPointsToCheck[0] /=  3.0f;

    CIntersactionInfo LocalInfo(intersectionInfo);

    const int nIterations = GetSettings()->m_nIterations;
    for (int i = 0, n = 4; i < n; ++i)
    {
        if (intersect(ray, LocalInfo, aPointsToCheck[i], nIterations, bDebug))
        {
            intersectionInfo = LocalInfo;
            return true;
        }
        if (!GetSettings()->m_bMultipleSeeds)
            break;
    }
    return false;
}

bool CBezierPatch::IntersectLowQuality(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug) const
{
    std::vector<QVector2D> aFirst;
    aFirst.push_back(QVector2D(3,0));
    aFirst.push_back(QVector2D(2,1));
    aFirst.push_back(QVector2D(1,2));
    aFirst.push_back(QVector2D(2,0));
    aFirst.push_back(QVector2D(1,1));
    aFirst.push_back(QVector2D(1,0));

    for(int i = 0, m = aFirst.size(); i < m; ++i)
    {
        int u = aFirst[i].x(), v = aFirst[i].y();
        CVector3DF vA = GetPoint(aFirst[i]);
        CUtils::GetNextPoint(u, v, 0, -1);
        CVector3DF vB = GetPoint(u, v);
        CUtils::GetNextPoint(u, v, 1, -1);
        CVector3DF vC = GetPoint(u, v);

        if (CUtils::IntersectTriangle(ray, intersectionInfo, vA, vB, vC))
        {
            return true;
        }
    }
    return false;
}

bool CBezierPatch::intersect(const CRay &ray, CIntersactionInfo &info, CVector3DF barCoord, unsigned int iterations, bool bDebug) const
{
    //Planes along ray
    CVector3DF N1 = ray.Direction().Cross(CVector3DF(-1.0f,-1.0f,-1.0f));
    CVector3DF N2 = ray.Direction().Cross(N1);
    const float d1 = -N1.Dot(ray.StartPoint());
    const float d2 = -N2.Dot(ray.StartPoint());

    float fSMall = 1e-3;

    CVector3DF dBu, dBv;

    for (unsigned int i = 0; i < iterations; i++)
    {
        const double &u(barCoord.X());
        const double &v(barCoord.Y());

        //partial U derivative of B
        dBu = GetdBu(u,v);

        //Partial V derivative of B
        dBv = GetdBv(u,v);

        const CVector3DF B = GetPointFromBarycentric(barCoord);

        const CVector3DF R = CVector3DF(N1.Dot(B) + d1,
                                        N2.Dot(B) + d2,
                                        0);

        if ( ( fabs(R.X()) < fSMall ) &&
             ( fabs(R.Y()) < fSMall ) )
        {
            break;
        }

        //Inverse Jacobian
        const float fN1dotdBu = N1.Dot(dBu);
        const float fN1dotdBv = N1.Dot(dBv);
        const float fN2dotdBu = N2.Dot(dBu);
        const float fN2dotdBv = N2.Dot(dBv);

        const float invConst = 1.0f / ( fN1dotdBu * fN2dotdBv - fN1dotdBv * fN2dotdBu );

        Matrix inverseJacobian;

        inverseJacobian[0][0] = fN2dotdBv  * invConst;
        inverseJacobian[0][1] = -fN2dotdBu * invConst;
        inverseJacobian[0][2] = 0;

        inverseJacobian[1][0] = -fN1dotdBv * invConst;
        inverseJacobian[1][1] = fN1dotdBu * invConst;
        inverseJacobian[1][2] = 0;

        inverseJacobian[2][0] = 0;
        inverseJacobian[2][1] = 0;
        inverseJacobian[2][2] = 1;

        //Newton Iteration

        barCoord = barCoord - R.MatrixMultiply(inverseJacobian);
        barCoord.SetZ(1.0 - barCoord.X() - barCoord.Y());

        if (barCoord.X() > k_fMAX || barCoord.X() < k_fMIN
                || barCoord.Y() > k_fMAX || barCoord.Y() < k_fMIN
                || barCoord.Z() > k_fMAX || barCoord.Z() < k_fMIN)
        {
            return false;
        }        
    }

    const float &u(barCoord.X());
    const float &v(barCoord.Y());
    const float w(1.0 - u - v);

    if (u < -k_fSMALL || u > 1.0f + k_fSMALL
            || v < -k_fSMALL || v > 1.0f + k_fSMALL
            || w < -k_fSMALL || w > 1.0f + k_fSMALL)
    {
        return false;
    }

    if (bDebug)
    {
        char str[100];
        sprintf( str, "u: %4.2f v: %4.2f w: %4.2f",  u, v, w);
        qDebug() << str;
    }

    //Calculating B
    const CVector3DF B = GetPointFromBarycentric(barCoord);

    if ( ( fabs(N1.Dot(B) + d1) > fSMall ) ||
         ( fabs(N2.Dot(B) + d2) > fSMall ) )
    {
        if (bDebug)
        {
            qDebug() << "error too big";
        }
        return false;
    }

    // calculate the intersection
    float len = (B - ray.StartPoint()).Length();
    if (len > info.m_fDistance)
    {
        if (bDebug)
        {
            qDebug() << "Len > Distance";
        }
        return false;
    }

    info.m_vIntersectionPoint = B;
    info.m_fDistance = len;

    info.m_vBarCoordsLocal.SetX(u);
    info.m_vBarCoordsLocal.SetY(v);
    info.m_vBarCoordsLocal.SetZ(w);

    info.m_vBarCoordsGlobal = info.m_vBarCoordsLocal;

    if (GetSettings()->m_bNormalSmoothing)
    {
        info.m_vNormal = GetSmoothedNormal(barCoord);
    }
    else
    {
//        info.m_vNormal = GetSmoothedNormal(barCoord);
        info.m_vNormal = CVector3DF::Normal(dBu, dBv);
    }

    if (bDebug)
    {
        qDebug()<< "Patch Hit!";
    }

    return true;
}

