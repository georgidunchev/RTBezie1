#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

//number of threads
static const uint k_nTHREADS = 4;

static const bool k_bUSE_KDTREE = true;
static const bool k_bSHOW_KDTREE = false;

//end criteria for kd-tree construction
static const int k_nMAX_LEVEL_OF_TREE = 20;
static const int k_nMIN_NUMBER_OF_TRIANGLES_TO_SPLIT = 6;

static const int k_nMAX_SPLITS_OF_NODE = 10;

static const int k_nNUMBER_OF_SUBDIVISIONS = 2;

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
