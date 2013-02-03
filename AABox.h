#ifndef AABOX_H
#define AABOX_H

#include <QObject>
#include <QVector3D>

class CRay;

class CAABox : public QObject
{
    Q_OBJECT
public:
    explicit CAABox(QObject *parent = 0);    
    explicit CAABox(const QVector3D & vMinVertex,const QVector3D & vMaxVertex, QObject *parent = 0);
    void Set(const QVector3D & vMinVertex,const QVector3D & vMaxVertex);

    bool IsInside(const QVector3D& vPoint) const;

    bool Intersect(const CRay& ray) const;
signals:
    
public slots:

private:
    QVector3D m_vMinVertex;
    QVector3D m_vMaxVertex;
};

#endif // AABOX_H
