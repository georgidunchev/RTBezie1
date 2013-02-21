#include "lightsscene.h"

#include <main.h>
#include <raytracer.h>
#include <light.h>
#include <lightpoint.h>

CLightsScene::CLightsScene()
{
    CLightPoint Light(QVector3D(0.f,1.f,-1.f));
    AddLight(Light);
}

CLightsScene::~CLightsScene()
{
    m_aLights.clear();
}

CMesh &CLightsScene::GetMesh()
{
    return GetRaytracer()->GetMesh();
}

void CLightsScene::AddLight(const ILight &Light)
{
    m_aLights.append(Light);
}

const ILight &CLightsScene::GetLight(int n)
{
    return m_aLights[n];
}

const int CLightsScene::GetLightsNumber()
{
    return m_aLights.size();
}
