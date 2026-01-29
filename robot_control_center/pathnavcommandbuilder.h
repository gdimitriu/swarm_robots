#ifndef PATHNAVCOMMANDBUILDER_H
#define PATHNAVCOMMANDBUILDER_H

#include <QString>

class PathNavCommandBuilder
{
public :
    virtual ~PathNavCommandBuilder() {}
    virtual QString deployFile(QString file) = 0;
    virtual QString deployEEPROM() = 0;
    virtual QString deployCommand(QString command) = 0;
    virtual QString saveToFile(QString file) = 0;
    virtual QString loadFromFile(QString file) = 0;
    virtual QString removeFile(QString file) = 0;
    virtual QString startDeploy() = 0;
    virtual QString runForward(QString file = nullptr) = 0;
    virtual QString runBackward(QString file = nullptr) = 0;
};

#endif // PATHNAVCOMMANDBUILDER_H
