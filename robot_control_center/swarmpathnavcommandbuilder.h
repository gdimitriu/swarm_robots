#ifndef SWARMPATHNAVCOMMANDBUILDER_H
#define SWARMPATHNAVCOMMANDBUILDER_H

#include "pathnavcommandbuilder.h"

class SwarmPathNavCommandBuilder : public PathNavCommandBuilder
{
public:
    SwarmPathNavCommandBuilder();
    virtual ~SwarmPathNavCommandBuilder();
    virtual QString deployFile(QString file);
    virtual QString deployEEPROM();
    virtual QString deployCommand(QString command);
    virtual QString saveToFile(QString file);
    virtual QString loadFromFile(QString file);
    virtual QString removeFile(QString file);
    virtual QString startDeploy();
    virtual QString runForward(QString file = nullptr);
    virtual QString runBackward(QString file = nullptr);
};

#endif // SWARMPATHNAVCOMMANDBUILDER_H
