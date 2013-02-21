#ifndef COLOR_H
#define COLOR_H

#include <QRgb>
class CColor
{
public:
    CColor(float fR, float fG, float fB);
    QRgb GetRGB() const;
    CColor operator *(const float &fMultiplier);
    CColor& operator *=(const float &fMultiplier);
    CColor operator +(const CColor &color);
    CColor& operator +=( CColor color);
private:
    float fRGB[3];
};

#endif // COLOR_H
