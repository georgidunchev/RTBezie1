#include "vector3df.h"

CVector3DF::CVector3DF()
{
}

CVector3DF::CVector3DF(float xpos, float ypos, float zpos)
    : m_vVector(xpos, ypos, zpos)
{

}

float CVector3DF::X() const
{
    return m_vVector.x();
}

float CVector3DF::Y() const
{
    return m_vVector.y();
}

float CVector3DF::Z() const
{
    return m_vVector.z();
}

void CVector3DF::SetX(float f)
{
    m_vVector.setX(f);
}

void CVector3DF::SetY(float f)
{
    m_vVector.setY(f);
}

void CVector3DF::SetZ(float f)
{
    m_vVector.setZ(f);
}

void CVector3DF::Normalize()
{
    m_vVector.normalize();
}

CVector3DF CVector3DF::Normalized() const
{
    return m_vVector.normalized();
}

float CVector3DF::Length() const
{
    return m_vVector.length();
}

float CVector3DF::GetDimension(EDimiensions eDimension) const
{
    switch(eDimension)
    {
    case e_Dimension_X:
    {
	return m_vVector.x();
    }
    case e_Dimension_Y:
    {
	return m_vVector.y();
    }
    case e_Dimension_Z:
    {
	return m_vVector.z();
    }
    }
    return 0.f;
}

void CVector3DF::SetDimension(CVector3DF::EDimiensions eDimension, const float fValue)
{
    switch(eDimension)
    {
    case e_Dimension_X:
    {
	m_vVector.setX(fValue);
	break;
    }
    case e_Dimension_Y:
    {
	m_vVector.setY(fValue);
	break;
    }
    case e_Dimension_Z:
    {
	m_vVector.setZ(fValue);
	break;
    }
    }
}

float CVector3DF::Dot(const CVector3DF &vOther) const
{
    return QVector3D::dotProduct(m_vVector, vOther.m_vVector);
}

CVector3DF CVector3DF::Cross(const CVector3DF &vOther) const
{
    return CVector3DF(QVector3D::crossProduct(m_vVector, vOther.m_vVector));
}

CVector3DF CVector3DF::MatrixMultiply(const Matrix &m) const
{
    return CVector3DF(
		m_vVector.x() * m[0][0] + m_vVector.y() * m[1][0] + m_vVector.z() * m[2][0],
		m_vVector.x() * m[0][1] + m_vVector.y() * m[1][1] + m_vVector.z() * m[2][1],
		m_vVector.x() * m[0][2] + m_vVector.y() * m[1][2] + m_vVector.z() * m[2][2] );
}

float CVector3DF::Cos(const CVector3DF &vOther) const
{
    return QVector3D::dotProduct(m_vVector.normalized(), vOther.m_vVector.normalized());
}

CVector3DF CVector3DF::ProjectionInPlane(const CVector3DF &vNormalOfPlane) const
{
    const CVector3DF vHelper = this->Cross(vNormalOfPlane);
    const CVector3DF vProjection = (vNormalOfPlane.Cross(vHelper)).m_vVector.normalized();
    const float fLen = this->Dot(vProjection);
    return vProjection * fLen;
}

CVector3DF CVector3DF::Normal(const CVector3DF &vAB, const CVector3DF &vAC)
{
    return vAB.Cross(vAC).m_vVector.normalized();
}

float CVector3DF::Triple(CVector3DF vec1, CVector3DF vec2, CVector3DF vec3)
{
    return vec1.Dot( vec2.Cross(vec3) );
}

CVector3DF::CVector3DF(const QVector3D &vOther)
    :m_vVector(vOther)
{

}

CVector3DF &CVector3DF::operator+=(const CVector3DF &vector)
{
    m_vVector += vector.m_vVector;
    return *this;
}

CVector3DF &CVector3DF::operator-=(const CVector3DF &vector)
{
    m_vVector -= vector.m_vVector;
    return *this;
}

CVector3DF &CVector3DF::operator*=(float factor)
{
    m_vVector *= factor;
    return *this;
}

CVector3DF &CVector3DF::operator*=(const CVector3DF& vector)
{
    m_vVector *= vector.m_vVector;
    return *this;
}

CVector3DF &CVector3DF::operator/=(float divisor)
{
    m_vVector /= divisor;
    return *this;
}

const CVector3DF operator+(const CVector3DF &v1, const CVector3DF &v2)
{
    return v1.m_vVector + v2.m_vVector;
}


const CVector3DF operator-(const CVector3DF &v1, const CVector3DF &v2)
{
    return v1.m_vVector - v2.m_vVector;
}

const CVector3DF operator*(float factor, const CVector3DF &vector)
{
    return CVector3DF(vector.X() * factor,
		      vector.Y() * factor,
		      vector.Z() * factor);
}

const CVector3DF operator*(const CVector3DF &vector, float factor)
{
    return CVector3DF(vector.X() * factor,
		      vector.Y() * factor,
		      vector.Z() * factor);
}


const CVector3DF operator*(const CVector3DF &v1, const CVector3DF &v2)
{
    return v1.m_vVector * v2.m_vVector;
}


const CVector3DF operator/(const CVector3DF &vector, float divisor)
{
    return vector.m_vVector / divisor;
}


const CVector3DF operator-(const CVector3DF &vector)
{
    return -vector.m_vVector;
}
