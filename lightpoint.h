#ifndef LIGNTPOINT_H
#define LIGNTPOINT_H
#include <light.h>

class CLightPoint : public ILight
{
public:
    CLightPoint(const QVector3D& vPosition);
};

#endif // LIGNTPOINT_H
