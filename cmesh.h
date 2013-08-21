#ifndef CMESH_H
#define CMESH_H

#include <QObject>
#include <ray.h>
#include <intersactioninfo.h>
#include <triangle.h>
#include <beziertriangle.h>
#include <Utils.h>
#include <vector>
#include <vertex.h>

class CKDTreeNode;

struct CSortedBBEntry;
struct SAdjacencyOfTriangle
{
    std::vector<int> aAdjacentTriangles;
    std::vector<int> aAdjacentVertices;
    enum EAdjTrianglesFlags
    {
	k_AdjTriangles_Has_A = 0x01,    //0001
	k_AdjTriangles_Has_B = 0x02,    //0010
	k_AdjTriangles_Has_C = 0x04,    //0100
	k_AdjTriangles_Complete = 0x07  //0111
    };
    unsigned int Flags;

    SAdjacencyOfTriangle()
	: Flags(0)
    {
	aAdjacentTriangles.resize(3);
	aAdjacentVertices.resize(3);
    };
    bool Complete(int nSide = -1) const;
    void SetComplete(int nSide);
};

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

    const SAdjacencyOfTriangle& GetAdjacentTriangles(int n);

    CBezierTriangle *GetBezierTriangle(int n);

    void GenerateKDTree();
    bool CompareBB(const CSortedBBEntry &s1, const CSortedBBEntry &s2);

    int GetKDTreeNextID();

    bool IntersectKDTree(const CRay &ray, CIntersactionInfo &intersectionInfo, bool bDebug = false);

signals:
    
public slots:

private:
    void SortBBoxes();
	void MakeBoundingBox();
    void BuildVertexData();

    void BuildAdjacency();
    int FindAdjacentTriangle(int nTriangleID, int nSide);

    void BuildBezierTriangles();

    std::vector<CVertex> m_aVertices;
    std::vector<CTriangle*> m_aTriangles;
    std::vector< SAdjacencyOfTriangle > m_aAdjacentTriangles;
    int m_nTrianglesWithCompleteAdjacency;
    std::vector<CTriangle*> m_aBezierTriangles;

    CAABox m_BoundingBox;

    CKDTreeNode * m_pRoot;
    std::vector<CSortedBBEntry> m_aSortedBBoxes[3];
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
