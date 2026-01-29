#ifndef MOVECOMMANDBUILDER_H
#define MOVECOMMANDBUILDER_H
#include <QString>
class MoveCommandBuilder
{
public:
    virtual ~MoveCommandBuilder() {}
    virtual QString moveOrRotate(QString length, QString angle) = 0;
    virtual QString stop() = 0;
    virtual QString getMinPower() = 0;
    virtual QString setMinPower(QString value) = 0;
    virtual QString getMaxPower() = 0;
    virtual QString setMaxPower(QString value) = 0;
    virtual QString getStopDistance() = 0;
    virtual QString setStopDistance(QString value) = 0;
    virtual QString getLowPowerDistance() = 0;
    virtual QString setLowPowerDistance(QString value) = 0;
    virtual QString getCurrentPower() = 0;
    virtual QString setCurrentPower(QString value) = 0;
};

#endif // MOVECOMMANDBUILDER_H
