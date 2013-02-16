#ifndef KDTREENODE_H
#define KDTREENODE_H

#include <QVector3D>
#include <Utils.h>
#include <AABox.h>
#include <ray.h>
#include <intersactioninfo.h>

class CMesh;

class CKDTreeNode
{
public:
    CKDTreeNode(QVector<int>* pTriangles, int nLevel, const CAABox& BoundingBox);
    ~CKDTreeNode();

    void Process();
    bool Separate(QVector<int>& AllTriangles,  QVector<int>* pLeftTriangleIndeces,  QVector<int>* pRightTriangleIndeces, EDimiensions eDimension, float& fBestDivide);

    bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo );
private:
    CMesh &GetMesh();
    QVector<int>* m_pTriangles;
    int m_nLevel;
    CKDTreeNode * m_pLeftNode;
    CKDTreeNode * m_pRightNode;
    CAABox m_BoundingBox;
};

#endif // KDTREENODE_H
