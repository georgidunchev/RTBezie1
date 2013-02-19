#ifndef SCENELIGHTS_H
#define SCENELIGHTS_H

#include <QVector>

#include <light.h>
#include <cmesh.h>

class CLightsScene
{
public:
    explicit CLightsScene();

    void AddLight(const ILight &Light);
private:
    CMesh &GetMesh();
    QVector<ILight> m_aLights;
};

#endif // SCENELIGHTS_H
