#include <Utils.h>
#include <ray.h>

CUtils::CUtils()
{
}

float CUtils::GetDimension(const QVector3D& pVector, EDimiensions eDimension)
{
    switch(eDimension)
    {
    case e_Dimension_X:
    {
	return pVector.x();
    }
    case e_Dimension_Y:
    {
	return pVector.y();
    }
    case e_Dimension_Z:
    {
	return pVector.z();
    }
    }
    return 0.f;
}

void CUtils::SetDimension(QVector3D &pVector, EDimiensions eDimension, const float fValue)
{
    switch(eDimension)
    {
    case e_Dimension_X:
    {
	pVector.setX(fValue);
	break;
    }
    case e_Dimension_Y:
    {
	pVector.setY(fValue);
	break;
    }
    case e_Dimension_Z:
    {
	pVector.setZ(fValue);
	break;
    }
    }
}

void CUtils::SafeDel(void *pointer)
{
    if (pointer)
    {
	delete pointer;
    }
}

QVector3D CUtils::GetPointAtDistance(const CRay &ray, float fDistance)
{
    QVector3D vNewPos = ray.Direction();
    vNewPos *= fDistance;
    vNewPos += ray.StartPoint();
    return vNewPos;
}

float  CUtils::Dot(QVector3D vec1, QVector3D vec2)
{
    return QVector3D::dotProduct(vec1, vec2);
}

QVector3D  CUtils::Cross(QVector3D vec1, QVector3D vec2)
{
    return QVector3D::crossProduct(vec1, vec2);
}

float CUtils::Triple(QVector3D vec1, QVector3D vec2, QVector3D vec3)
{
    return Dot(vec1, Cross(vec2, vec3) );
}
