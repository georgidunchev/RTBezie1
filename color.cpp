#include "color.h"

CColor::CColor(float fR, float fG, float fB)
{
    aRGB.resize(3);
    aRGB[0] = fR;
    aRGB[1] = fG;
    aRGB[2] = fB;
}

CColor::CColor(const CColor &rhs)
{
    this->aRGB = rhs.aRGB;
}

CColor::CColor(const QRgb &rhs)
{
    aRGB.push_back(static_cast<float>(qRed(rhs)) / 255.0f );
    aRGB.push_back(static_cast<float>(qGreen(rhs)) / 255.0f);
    aRGB.push_back(static_cast<float>(qBlue(rhs)) / 255.0f);
}

CColor::CColor()
{
    aRGB.resize(3);
    aRGB[0] = 0.0f;
    aRGB[1] = 0.0f;
    aRGB[2] = 0.0f;
}

QRgb CColor::GetRGB() const
{
    CColor clamped = *this;

    clamped.aRGB[0] = clamped.aRGB[0]>1.f?1.f:clamped.aRGB[0];
    clamped.aRGB[1] = clamped.aRGB[1]>1.f?1.f:clamped.aRGB[1];
    clamped.aRGB[2] = clamped.aRGB[2]>1.f?1.f:clamped.aRGB[2];

    return qRgb(clamped.aRGB[0] * 255.0f,
		clamped.aRGB[1] * 255.0f,
		clamped.aRGB[2] * 255.0f);
}

CColor &CColor::operator =(const CColor &rhs)
{
    aRGB = rhs.aRGB;
    return *this;
}

CColor &CColor::operator =(const QRgb &rhs)
{
    aRGB[0] = static_cast<float>(qRed(rhs)) / 255.0f ;
    aRGB[1] = static_cast<float>(qGreen(rhs)) / 255.0f;
    aRGB[2] = static_cast<float>(qBlue(rhs)) / 255.0f;
    return *this;
}

CColor CColor::operator *(const float &fMultiplier)
{
    return CColor(aRGB[0]*fMultiplier,
		  aRGB[1]*fMultiplier,
		  aRGB[2]*fMultiplier);
}

CColor &CColor::operator *=(const float &fMultiplier)
{
    aRGB[0]*=fMultiplier;
    aRGB[1]*=fMultiplier;
    aRGB[2]*=fMultiplier;
    return *this;
}

CColor CColor::operator +(const CColor &color)
{
    return CColor(aRGB[0]+color.aRGB[0],
		  aRGB[1]+color.aRGB[1],
		  aRGB[2]+color.aRGB[2]);
}

CColor& CColor::operator +=(CColor color)
{
    aRGB[0] += color.aRGB[0];
    aRGB[1] += color.aRGB[1];
    aRGB[2] += color.aRGB[2];
    return *this;
}
