#include "jsoncamera.h"

JsonCamera::JsonCamera(QObject *parent) : QObject(parent)
{

}

JsonCamera::JsonCamera(QObject *parent, QString present, QString port, QString protocol) : QObject(parent)
{
    this->present = present;
    this->port = port;
    this->protocol = protocol;
}

void JsonCamera::read(const QJsonObject &json)
{
    present = json["present"].toString();
    if ( present.compare("yes",Qt::CaseInsensitive) == 0 )
    {
        port = json["port"].toString();
        protocol = json["protocol"].toString();
        hasStartStop = json["startstop"].toString();
    }
    else
    {
        port.clear();
        protocol.clear();
    }

}

void JsonCamera::write(QJsonObject &json) const
{
    json["present"] = present;
    if ( present.compare("yes",Qt::CaseInsensitive) == 0 )
    {
        json["port"] = port;
        json["protocol"] = protocol;
        json["startstop"] = hasStartStop;
    }
}

QString JsonCamera::isPresent() const
{
    return present;
}

QString JsonCamera::getPort() const
{
    return port;
}

QString JsonCamera::getProtocol() const
{
    return protocol;
}

QString JsonCamera::startStopCommand() const
{
    return hasStartStop;
}

void JsonCamera::setPresent(QString present)
{
    this->present = present;
}

void JsonCamera::setPort(QString port)
{
    this->port = port;
}

void JsonCamera::setProtocol(QString protocol)
{
    this->protocol = protocol;
}

void JsonCamera::setStartStopCommand(QString hasStartStop)
{
    this->hasStartStop = hasStartStop;
}
