#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
static const float k_fMAX = 1e9;
static const float k_fSMALL = 1e-9;
static const float k_fMIN = -k_fMAX;

//end criteria for kd-tree construction
static const int k_nMAX_LEVEL_OF_TREE = 10;
static const int k_nMIN_NUMBER_OF_TRIANGLES_TO_SPLIT = 4;

static const int k_nMAX_SPLITS_OF_NODE = 10;

class CSettings : public QObject
{
    Q_OBJECT
public:
    explicit CSettings(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // SETTINGS_H
