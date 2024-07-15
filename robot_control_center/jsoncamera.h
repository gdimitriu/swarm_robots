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
    QString getPort() const;
    QString getProtocol() const;
    QString startStopCommand() const;
private:
    QString present;
    QString port;
    QString protocol;
    QString hasStartStop;

};

#endif // JSONCAMERA_H
