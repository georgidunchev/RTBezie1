#ifndef COLOR_H
#define COLOR_H

#include <vector>
#include <QRgb>
class CColor
{
public:
    CColor(float fR, float fG, float fB);
    CColor(const CColor& rhs);
    CColor(const QRgb& rhs);
    CColor();

    QRgb GetRGB() const;

    CColor& operator =(const CColor &rhs);
    CColor& operator =(const QRgb &rhs);

    CColor operator *(const float &fMultiplier);
    CColor& operator *=(const float &fMultiplier);
    CColor operator +(const CColor &color);
    CColor& operator +=( CColor color);
private:
    std::vector<float> aRGB;
};

#endif // COLOR_H
