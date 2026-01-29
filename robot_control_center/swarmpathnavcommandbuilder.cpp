#include "swarmpathnavcommandbuilder.h"

SwarmPathNavCommandBuilder::SwarmPathNavCommandBuilder()
{

}

SwarmPathNavCommandBuilder::~SwarmPathNavCommandBuilder()
{

}

QString SwarmPathNavCommandBuilder::deployFile(QString file)
{
    QString message("Nfb");
    message.append(file);
    message.append("#");
    return message;
}
QString SwarmPathNavCommandBuilder::deployEEPROM()
{
    return QString();
}
QString SwarmPathNavCommandBuilder::deployCommand(QString command)
{
    QString message("N");
    message.append(command);
    return message;
}
QString SwarmPathNavCommandBuilder::startDeploy()
{
    return QString("n#");
}

QString SwarmPathNavCommandBuilder::saveToFile(QString file)
{
    QString message("Nfs");
    message.append(file);
    message.append("#");
    return message;
}
QString SwarmPathNavCommandBuilder::loadFromFile(QString file)
{
    QString message("Nfl");
    message.append(file);
    message.append("#");
    return message;
}
QString SwarmPathNavCommandBuilder::removeFile(QString file)
{
    QString message("Nfr");
    message.append(file);
    message.append("#");
    return message;
}
QString SwarmPathNavCommandBuilder::runForward(QString file)
{

    if (file == nullptr)
    {
        return QString("D#");
    }
    else
    {
        QString message("ND");
        message.append(file);
        message.append("#");
        return message;
    }
}
QString SwarmPathNavCommandBuilder::runBackward(QString file)
{
    if (file == nullptr)
    {
        return QString("B#");
    }
    else
    {
        QString message("NB");
        message.append(file);
        message.append("#");
        return message;
    }
}
