#ifndef KDTREENODE_H
#define KDTREENODE_H

#include <QVector3D>
#include <Utils.h>
#include <AABox.h>
#include <ray.h>
#include <intersactioninfo.h>
#include <vector>

class CMesh;

class CKDTreeNode
{
public:
    CKDTreeNode(std::vector<int>* pTriangles, int nLevel, const CAABox& BoundingBox);
    ~CKDTreeNode();

    void Process();
    bool Separate(std::vector<int>& AllTriangles,  std::vector<int>* pLeftTriangleIndeces,  std::vector<int>* pRightTriangleIndeces, EDimiensions eDimension, float& fBestDivide);

    bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo );
private:
    CMesh &GetMesh();
    std::vector<int>* m_pTriangles;
    int m_nLevel;
    CKDTreeNode * m_pLeftNode;
    CKDTreeNode * m_pRightNode;
    CAABox m_BoundingBox;
};

#endif // KDTREENODE_H
