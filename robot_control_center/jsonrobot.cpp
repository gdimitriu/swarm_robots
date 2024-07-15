#include "jsonrobot.h"

JsonRobot::JsonRobot(QObject *parent) : QObject(parent)
{
    camera = nullptr;
}

JsonRobot::JsonRobot(QObject *parent, QString name, QString ip,QString port, JsonCamera *camera) : QObject(parent)
{
    this->name = name;
    this->ip = ip;
    this->port = port;
    this->camera = camera;
}

void JsonRobot::read(const QJsonObject &json)
{
    name = json["name"].toString();
    ip = json["ip"].toString();
    port = json["port"].toString();
    QJsonObject cameraObject = json["camera"].toObject();
    if ( camera != nullptr )
        delete camera;
    camera = new JsonCamera(this);
    camera->read(cameraObject);
}

void JsonRobot::write(QJsonObject &json) const
{
    json["name"] = name;
    json["ip"] = ip;
    json["port"] = port;
    QJsonObject object;
    camera->write(object);
    json["camera"] = object;
}

QString JsonRobot::getName() const
{
    return name;
}

QString JsonRobot::getIp() const
{
    return ip;
}

QString JsonRobot::getPort() const
{
    return port;
}

QString JsonRobot::hasCamera() const
{
    if ( camera == nullptr )
        return QString("no");
    return camera->isPresent();
}

QString JsonRobot::cameraPort() const
{
    if ( camera == nullptr )
        return QString();
    return camera->getPort();
}

QString JsonRobot::cameraProtocol() const
{
    if ( camera == nullptr )
        return QString();
    return camera->getProtocol();
}

QString JsonRobot::hasStartStop() const
{
    if ( camera == nullptr)
        return QString("No");
    return camera->startStopCommand();
}
