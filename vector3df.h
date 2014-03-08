#ifndef VECTOR3DF_H
#define VECTOR3DF_H

#include <QVector3D>

typedef float Matrix [3][3];

class CVector3DF
{
public:
    enum EDimiensions
    {
	e_Dimension_X = 0,
	e_Dimension_Y,
	e_Dimension_Z,
	e_Dimension_MAX
    };

    CVector3DF();
    CVector3DF(float xpos, float ypos, float zpos);
    CVector3DF(const QVector3D& vOther);

    float X() const;
    float Y() const;
    float Z() const;

    void SetX(float f);
    void SetY(float f);
    void SetZ(float f);

    void Normalize();
    CVector3DF Normalized() const;

    float Length() const;

    float GetDimension(EDimiensions eDimension) const;
    void SetDimension(EDimiensions eDimension, const float fValue);

    float Dot(const CVector3DF& vOther) const;
    CVector3DF Cross(const CVector3DF& vOther) const;

    CVector3DF MatrixMultiply(const Matrix &m) const;

    float Cos(const CVector3DF& vOther) const;
    CVector3DF ProjectionInPlane(const CVector3DF& vNormalOfPlane) const;

    static CVector3DF Normal(const CVector3DF& vAB, const CVector3DF& vAC);

    static float Triple(CVector3DF vec1, CVector3DF vec2, CVector3DF vec3);


    CVector3DF &operator+=(const CVector3DF &vector);
    CVector3DF &operator-=(const CVector3DF &vector);
    CVector3DF &operator*=(float factor);
    CVector3DF &operator*=(const CVector3DF& vector);
    CVector3DF &operator/=(float divisor);

    friend const CVector3DF operator*(float factor, const CVector3DF &vector);
    friend const CVector3DF operator*(const CVector3DF &vector, float factor);
    friend const CVector3DF operator+(const CVector3DF &v1, const CVector3DF &v2);
    friend const CVector3DF operator-(const CVector3DF &v1, const CVector3DF &v2);
    friend const CVector3DF operator-(const CVector3DF &vector);
    friend const CVector3DF operator*(const CVector3DF &v1, const CVector3DF& v2);
    friend const CVector3DF operator/(const CVector3DF &vector, float divisor);

     const QVector3D& GetQVector() const
     {
         return m_vVector;
     }

private:
    QVector3D m_vVector;
};




#endif // VECTOR3DF_H
