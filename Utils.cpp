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

QVector3D CUtils::VertexMatrixMultiply(const QVector3D& v, const Matrix& m)
{
	return QVector3D(
		v.x() * m[0][0] + v.y() * m[1][0] + v.z() * m[2][0],
		v.x() * m[0][1] + v.y() * m[1][1] + v.z() * m[2][1],
		v.x() * m[0][2] + v.y() * m[1][2] + v.z() * m[2][2]
	);
}
