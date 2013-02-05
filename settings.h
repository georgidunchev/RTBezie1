#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
static const float k_fMAX = 1e9;
static const float k_fSMALL = 1e-9;
static const float k_fMIN = -k_fMAX;

class CSettings : public QObject
{
    Q_OBJECT
public:
    explicit CSettings(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // SETTINGS_H
