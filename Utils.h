#ifndef UTILS
#define UTILS

#include <QVector3D>

enum EDimiensions
{
    e_Dimension_X = 0,
    e_Dimension_Y,
    e_Dimension_Z,
};

class CUtils
{
public:
    CUtils();
    static float GetDimension(const QVector3D& pVector, EDimiensions eDimension);
};

#endif // UTILS
