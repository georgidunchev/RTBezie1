#ifndef UTILS
#define UTILS

#include <vector3df.h>
class CRay;
class CIntersactionInfo;

static const float k_fMAX = 1e6f;
static const float k_fSMALL = 1.0f / 1e6f;
static const float k_fMIN = -k_fMAX;

static const float k_fPI = 3.14159;
static const float k_fDegreesToRadians = k_fPI/180.0;
static const float	k_fOneThird = 1.0f/3.0f;

class CUtils
{
public:
    CUtils();

    static void SafeDel(void * pointer);

    static void TriangleCentre( CVector3DF &o_vCentre,
				const CVector3DF& i_vA,
				const CVector3DF& i_vB,
				const CVector3DF& i_vC);

    static bool IntersectTriangle( const CRay &i_Ray,
				   CIntersactionInfo& io_IntersectionInfo,
				   const CVector3DF& i_vA,
				   const CVector3DF& i_vB,
				   const CVector3DF& i_vC);

    static bool IntersectTriangle( const CRay &i_Ray,
				   CIntersactionInfo& io_IntersectionInfo,
				   const CVector3DF& i_vA,
				   const CVector3DF& i_vB,
				   const CVector3DF& i_vC,
				   const CVector3DF& i_vABar,
				   const CVector3DF& i_vBBar,
				   const CVector3DF& i_vCBar);

    static int PowerOf2(const int nPow);

    static CVector3DF Reflect(const CVector3DF& toBeReflected, const CVector3DF& normal);

    static void GetNextPoint(int& io_a, int& io_b, int nPos, int nMod);
};

#endif // UTILS
