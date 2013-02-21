#include "color.h"

CColor::CColor(float fR, float fG, float fB)
{
    fRGB[0] = fR;
    fRGB[1] = fG;
    fRGB[2] = fB;
}

QRgb CColor::GetRGB() const
{
    CColor clamped = *this;

    clamped.fRGB[0] = clamped.fRGB[0]>1.f?1.f:clamped.fRGB[0];
    clamped.fRGB[1] = clamped.fRGB[1]>1.f?1.f:clamped.fRGB[1];
    clamped.fRGB[2] = clamped.fRGB[2]>1.f?1.f:clamped.fRGB[2];

    return qRgb(clamped.fRGB[0] * 255,
		clamped.fRGB[1] * 255,
		clamped.fRGB[2] * 255);
}

CColor CColor::operator *(const float &fMultiplier)
{
    return CColor(fRGB[0]*fMultiplier,
		  fRGB[1]*fMultiplier,
		  fRGB[2]*fMultiplier);
}

CColor &CColor::operator *=(const float &fMultiplier)
{
    fRGB[0]*=fMultiplier;
    fRGB[1]*=fMultiplier;
    fRGB[2]*=fMultiplier;
    return *this;
}

CColor CColor::operator +(const CColor &color)
{
    return CColor(fRGB[0]+color.fRGB[0],
		  fRGB[1]+color.fRGB[1],
		  fRGB[2]+color.fRGB[2]);
}

CColor& CColor::operator +=(CColor color)
{
    fRGB[0] += color.fRGB[0];
    fRGB[1] += color.fRGB[1];
    fRGB[2] += color.fRGB[2];
    return *this;
}
