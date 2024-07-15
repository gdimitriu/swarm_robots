#ifndef JSONROBOT_H
#define JSONROBOT_H

#include <QObject>
#include <QJsonObject>
#include "jsoncamera.h"

class JsonRobot : public QObject
{
    Q_OBJECT
public:
    JsonRobot(QObject *parent = nullptr);
    JsonRobot(QObject *parent, QString name, QString ip,QString port, JsonCamera *camera);
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
    QString getName() const;
    QString getIp() const;
    QString getPort() const;
    QString hasCamera() const;
    QString cameraPort() const;
    QString cameraProtocol() const;
    QString hasStartStop() const;
private:
    QString name;
    QString ip;
    QString port;
    JsonCamera *camera;
};

#endif // JSONROBOT_H
