#ifndef UTILS
#define UTILS

#include <QVector3D>
class  CRay;

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
    static void SafeDel(void * pointer);

    static QVector3D GetPointAtDistance(const CRay &ray, float fDistance);

    static float Dot(QVector3D vec1, QVector3D vec2);
    static QVector3D Cross(QVector3D vec1, QVector3D vec2);
    static float Triple(QVector3D vec1, QVector3D vec2, QVector3D vec3);
};




#endif // UTILS
