#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <QObject>
#include <QtGlobal>
#include <QImage>

#include <camera.h>

class CMesh;

class RayTracer : public QObject
{
    Q_OBJECT
public:
    explicit RayTracer(QObject *parent = 0);
    void SetCanvas(qreal fWidth, qreal fHeight);
    void Render();
    ~RayTracer();
    const QImage &GetImage() const;

    Camera & GetCamera();
private:
    Camera m_Camera;

    QImage * m_pImage;
    qreal m_fWidth;
    qreal m_fHeight;

    CMesh* m_Mesh;

signals:

public slots:
};

#endif // RAYTRACER_H
