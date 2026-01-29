#include "swarmcameracommandbuilder.h"

SwarmCameraCommandBuilder::SwarmCameraCommandBuilder()
{

}

SwarmCameraCommandBuilder::~SwarmCameraCommandBuilder()
{

}
QString SwarmCameraCommandBuilder::moveLeft(QString value)
{
    QString message("Tx-");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmCameraCommandBuilder::moveRight(QString value)
{
    QString message("Tx");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmCameraCommandBuilder::moveUp(QString value)
{
    QString message("Ty");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmCameraCommandBuilder::moveDown(QString value)
{
    QString message("Ty");
    message.append(value);
    message.append("#");
    return message;
}
QString SwarmCameraCommandBuilder::moveCenterHorinzonal()
{
    QString message;

    return message;
}
QString SwarmCameraCommandBuilder::moveCenterVertical()
{
    QString message;

    return message;
}

QString SwarmCameraCommandBuilder::startStreaming()
{
    return QString("T#");
}
QString SwarmCameraCommandBuilder::stopStraming()
{
    return QString("t#");
}
