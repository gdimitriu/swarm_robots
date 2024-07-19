#ifndef JSONCAMERA_H
#define JSONCAMERA_H

#include <QObject>
#include <QJsonObject>

class JsonCamera : public QObject
{
    Q_OBJECT
public:
    JsonCamera(QObject *parent = nullptr);
    JsonCamera(QObject *parent, QString present, QString port = nullptr, QString protocol = nullptr);
    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;
    QString isPresent() const;
    void setPresent(QString present);
    QString getPort() const;
    void setPort(QString port);
    QString getProtocol() const;
    void setProtocol(QString protocol);
    QString startStopCommand() const;
    void setStartStopCommand(QString hasStartStop);
private:
    QString present;
    QString port;
    QString protocol;
    QString hasStartStop;

};

#endif // JSONCAMERA_H
