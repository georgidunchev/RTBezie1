#ifndef SCENELIGHTS_H
#define SCENELIGHTS_H

#include <QVector>

#include <light.h>
#include <cmesh.h>

class CLightsScene
{
public:
    explicit CLightsScene();
    virtual ~CLightsScene();

    void AddLight(const ILight &Light);
    const ILight& GetLight(int n);
    const int GetLightsNumber();
private:
    CMesh &GetMesh();
    QVector<ILight> m_aLights;
};

#endif // SCENELIGHTS_H
