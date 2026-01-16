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
    ~JsonRobot();
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
    QString getName() const;
    void setName(QString name);
    QString getIp() const;
    void setIp(QString ip);
    QString getPort() const;
    void setPort(QString port);
    void setCamera(JsonCamera *camera);
    QString hasCamera() const;
    QString cameraPort() const;
    QString cameraProtocol() const;
    QString hasStartStop() const;
    QString hasMovingCamera() const;
private:
    QString name;
    QString ip;
    QString port;
    JsonCamera *camera;
};

#endif // JSONROBOT_H
