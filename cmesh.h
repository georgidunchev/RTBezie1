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
    bool Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, const QVector<int>& aTriangles, CAABox *pBBox = NULL);

    CTriangle& GetTriangle(int n);

    void GenerateKDTree();
    bool CompareBB(const CSortedBBEntry &s1, const CSortedBBEntry &s2);

    int GetKDTreeNextID();

    bool IntersectKDTree(const CRay &ray, CIntersactionInfo &intersectionInfo);
signals:
    
public slots:

private:
    void SortBBoxes();

    QVector<QVector3D> m_aVertices;
    QVector<CTriangle> m_aTriangles;

    CAABox mBoundingBox;

    CKDTreeNode * m_pRoot;
    QVector<CSortedBBEntry> m_aSortedBBoxes[3];
    EDimiensions m_eSortingDimention;
};

struct CSortedBBEntry
{
    bool m_bStart;
    int m_nTriangleId;

    static bool compare_X(const CSortedBBEntry &that, const CSortedBBEntry &other);
    static bool compare_Y(const CSortedBBEntry &that, const CSortedBBEntry &other);
    static bool compare_Z(const CSortedBBEntry &that, const CSortedBBEntry &other);
    static bool compare(const CSortedBBEntry &that, const CSortedBBEntry &other, EDimiensions eSortingDimention);
};



#endif // CMESH_H
