#ifndef UTILS
#define UTILS

#include <QVector3D>

enum EDimiensions
{
    e_Dimension_X = 0,
    e_Dimension_Y,
    e_Dimension_Z,
    e_Dimension_MAX
};

class CUtils
{
public:
    CUtils();
    static float GetDimension(const QVector3D& pVector, EDimiensions eDimension);
    static void SetDimension(QVector3D& pVector, EDimiensions eDimension, const float fValue);
};

#endif // UTILS
