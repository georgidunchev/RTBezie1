#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
static const float k_fMAX = 1e9;
static const float k_fSMALL = 1e-9;
static const float k_fMIN = -k_fMAX;

//number of threads
static const int k_nTHREADS = 4;

static const bool k_bUSE_KDTREE = false;

//end criteria for kd-tree construction
static const int k_nMAX_LEVEL_OF_TREE = 32;
static const int k_nMIN_NUMBER_OF_TRIANGLES_TO_SPLIT = 64;

static const int k_nMAX_SPLITS_OF_NODE = 10;

class CSettings : public QObject
{
    Q_OBJECT
public:
    explicit CSettings(QObject *parent = 0);
    
    void SetIntersectBezier(bool bIntersect);
    bool GetIntersectBezier();

    void SetNormalSmoothing(bool bSmooth);
    bool GetNormalSmoothing();

private:
    bool m_bINTERSECT_BEZIER;
    bool m_bNormalSmoothing;
signals:
    
public slots:
    
};

#endif // SETTINGS_H
