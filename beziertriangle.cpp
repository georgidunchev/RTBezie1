#include "beziertriangle.h"
#include <raytracer.h>
#include <main.h>
#include <Utils.h>
#include "cmesh.h"

CBezierTriangle::CBezierTriangle()
{
}

CBezierTriangle::CBezierTriangle(int nMainTriangleID)
{
    m_aVertecis = GetRaytracer()->GetMesh().GetVertices();
    CTriangle& Triangle = GetRaytracer()->GetMesh().GetTriangles()[nMainTriangleID];
    const SAdjacencyOfTriangle& AdjTriangles = GetRaytracer()->GetMesh().GetAdjacentTriangles(nMainTriangleID);

    m_aVertIndices.append(Triangle.Vertices()[0]);
    m_aVertIndices.append(Triangle.Vertices()[1]);
    m_aVertIndices.append(Triangle.Vertices()[2]);
    m_aVertIndices.append(AdjTriangles.aAdjacentVertices[0]);
    m_aVertIndices.append(AdjTriangles.aAdjacentVertices[1]);
    m_aVertIndices.append(AdjTriangles.aAdjacentVertices[2]);


    m_bHasBoundingBox = false;
    m_vAB = B() - A();
    m_vAC = C() - A();
    m_vNormal = CUtils::Cross(AB(), AC()).normalized();
}

bool CBezierTriangle::Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo) const
{
    return CTriangle::Intersect(ray, intersectionInfo);
}

bool CBezierTriangle::Intersect(const QVector3D &vStart, const QVector3D &vEnd) const
{
    return CTriangle::Intersect(vStart, vEnd);
}
