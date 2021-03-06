#ifndef LIGHT_H
#define LIGHT_H

#include <QVector3D>

class ILight
{
public:
    ILight();
    void SetPosition(const QVector3D& vPosition);
    const QVector3D& GetPosition() const;
private:
    QVector3D m_vPosition;
};

#endif // LIGHT_H
