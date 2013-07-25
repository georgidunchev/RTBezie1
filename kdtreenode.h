#ifndef KDTREENODE_H
#define KDTREENODE_H

#include <QVector3D>
#include <Utils.h>
#include <AABox.h>
#include <ray.h>
#include <intersactioninfo.h>
#include <vector>
#include "SubTriangle.h"

class CMesh;

class CKDTreeNode
{
public:
    CKDTreeNode(std::vector<CSubTriangle*>* pTriangles, int nLevel, const CAABox& BoundingBox);
    ~CKDTreeNode();

    void Process();
    bool Separate(std::vector<CSubTriangle*>& AllTriangles,
					std::vector<CSubTriangle*>* pLeftTriangleIndeces,
					std::vector<CSubTriangle*>* pRightTriangleIndeces,
					EDimiensions eDimension,
					float& fBestPortion);

    bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo );
private:
    CMesh &GetMesh();
    std::vector<CSubTriangle*>* m_pTriangles;
    int m_nLevel;
    CKDTreeNode * m_pLeftNode;
    CKDTreeNode * m_pRightNode;
    CAABox m_BoundingBox;
};

#endif // KDTREENODE_H
