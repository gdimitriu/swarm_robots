#ifndef SWARMMOVECOMMANDBUILDER_H
#define SWARMMOVECOMMANDBUILDER_H

#include "movecommandbuilder.h"

class SwarmMoveCommandBuilder : public MoveCommandBuilder
{
public:
    SwarmMoveCommandBuilder();
    virtual ~SwarmMoveCommandBuilder();
    virtual QString moveOrRotate(QString length, QString angle);
    virtual QString stop();
    virtual QString getMinPower();
    virtual QString setMinPower(QString value);
    virtual QString getMaxPower();
    virtual QString setMaxPower(QString value);
    virtual QString getStopDistance();
    virtual QString setStopDistance(QString value);
    virtual QString getLowPowerDistance();
    virtual QString setLowPowerDistance(QString value);
    virtual QString getCurrentPower();
    virtual QString setCurrentPower(QString value);
};

#endif // SWARMMOVECOMMANDBUILDER_H
