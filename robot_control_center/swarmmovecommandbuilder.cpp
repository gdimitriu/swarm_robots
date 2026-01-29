#include "swarmmovecommandbuilder.h"

SwarmMoveCommandBuilder::SwarmMoveCommandBuilder() {}
SwarmMoveCommandBuilder::~SwarmMoveCommandBuilder()
{

}
QString SwarmMoveCommandBuilder::moveOrRotate(QString length, QString angle)
{
    QString message("M");
    message.append(length);
    message.append(",");
    message.append(angle);
    message.append("#");
    return message;
}

QString SwarmMoveCommandBuilder::stop()
{
    return QString("b#");
}
QString SwarmMoveCommandBuilder::getMinPower()
{
    return QString("v#");
}
QString SwarmMoveCommandBuilder::setMinPower(QString value)
{
    QString message("v");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmMoveCommandBuilder::getMaxPower()
{
    return QString("V#");
}
QString SwarmMoveCommandBuilder::setMaxPower(QString value)
{
    QString message("V");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmMoveCommandBuilder::getStopDistance()
{
    return QString("s#");
}
QString SwarmMoveCommandBuilder::setStopDistance(QString value)
{
    QString message("s");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmMoveCommandBuilder::getLowPowerDistance()
{
    return QString("d#");
}
QString SwarmMoveCommandBuilder::setLowPowerDistance(QString value)
{
    QString message("d");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmMoveCommandBuilder::getCurrentPower()
{
    return QString("c#");
}
QString SwarmMoveCommandBuilder::setCurrentPower(QString value)
{
    QString message("c");
    message.append(value);
    message.append("#");
}
