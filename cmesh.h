#ifndef CMESH_H
#define CMESH_H

#include <QObject>
#include "ray.h"
#include "intersactioninfo.h"
#include "triangle.h"
#include "Utils.h"
#include "vector"
#include "vertex.h"
#include "vector3df.h"

class CKDTreeNode;

class CMesh : public QObject
{
    Q_OBJECT
public:
    explicit CMesh(QObject *parent = 0);
    
    void Load(const QString &strInputFileName);

	void ReadFromFile(const QString &strInputFileName);

    bool Intersect(const CRay& ray, CIntersactionInfo& intersectionInfo, bool bDebug = false);
    bool Intersect(const CRay &ray, CIntersactionInfo &intersectionInfo, const std::vector<int>& aTriangles, CAABox *pBBox = NULL, bool bDebug = false);

    std::vector<CTriangle*> *GetPrimitives();
    CTriangle* GetPrimitive(int n);
    std::vector<CTriangle *> &GetTriangles();
    const std::vector<CVertex> &GetVertices() const;
    std::vector<CVertex> &Vertices();

    void GenerateKDTree();

    int GetKDTreeNextID();

    bool IntersectKDTree(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug = false);

    const CAABox& GetAABB() const
    {
        return m_BoundingBox;
    }

    int m_nNOfSubtriangles;
    int n_mLeafs;

    int GetMemorySubKB() const;
    int GetMemoryKDtreeKB() const;

    bool isMeshLoaded() const;

signals:
    void sigLoadingStarted(int value);
    void sigLoadingStepDone(int value);
    void sigLoadingFinished();

    void sigLoadedMesh(bool value);

public slots:

private:
    void SortBBoxes();
	void MakeBoundingBox();
    void BuildVertexData();

    void BuildBezierTriangles();

    std::vector<CVertex> m_aVertices;
    std::vector<CTriangle*> m_aTriangles;

    CAABox m_BoundingBox;

    CKDTreeNode * m_pRoot;
    CVector3DF::EDimiensions m_eSortingDimention;
    bool m_bLoadedMesh;
};


#endif // CMESH_H
