#ifndef CMESH_H
#define CMESH_H

#include <QObject>
#include <QVector>
#include <ray.h>
#include <intersactioninfo.h>
#include <triangle.h>
#include <Utils.h>

class CKDTreeNode;

struct CSortedBBEntry;

class CMesh : public QObject
{
    Q_OBJECT
public:
    explicit CMesh(QObject *parent = 0);
    
    void Load(const QString &strInputFileName);

    bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo );

    CTriangle& GetTriangle(int n);

    void GenerateKDTree();
    bool CompareBB(const CSortedBBEntry &s1, const CSortedBBEntry &s2);
signals:
    
public slots:

private:
    void SortBBoxes();

    QVector<QVector3D> m_aVertices;
    QVector<CTriangle> m_aTriangles;

    CKDTreeNode * m_pRoot;
    QVector<CSortedBBEntry> m_aSortedBBoxes[3];
    EDimiensions m_eSortingDimention;
};

struct CSortedBBEntry
{
    static CMesh* m_pMesh;
    static EDimiensions m_eSortingDimention;

    bool m_bStart;
    int m_nTriangleId;

    bool operator <(const CSortedBBEntry& s2) const;
};



#endif // CMESH_H
