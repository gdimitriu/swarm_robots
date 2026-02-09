#ifndef SOCKETOPERATIONS_H
#define SOCKETOPERATIONS_H

#include <QTcpSocket>
class SocketOperations
{
public:
    SocketOperations(QTcpSocket *socket);
    ~SocketOperations();
    void disconnect();
    QString sendOneWay(QString message, bool hasAck = false);
    QString sendWithReply(QString message);
private:
    QTcpSocket *socket;
};

#endif // SOCKETOPERATIONS_H
