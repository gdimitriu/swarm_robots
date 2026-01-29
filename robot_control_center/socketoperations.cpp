#include "socketoperations.h"

SocketOperations::SocketOperations(QTcpSocket *socket)
{
    this->socket = socket;
}

void SocketOperations::disonnect()
{

}

QString SocketOperations::sendOneWay(QString message, bool hasAck)
{
    QString ackMessage;
    socket->write(message.toLatin1());
    socket->flush();
    if (hasAck)
    {
        socket->waitForReadyRead();
        QByteArray readData = socket->readAll();
        QString str(readData);
        while ( !(str.contains("OK\r\n") || str.contains("OK\n")) )
        {
            socket->waitForReadyRead();
            str.append(socket->readAll());
        }
    }
    return ackMessage;
}

QString SocketOperations::sendWithReply(QString message)
{
    socket->write(message.toLatin1());
    socket->flush();
    socket->waitForReadyRead();
    QByteArray readData = socket->readAll();
    while ( socket->bytesAvailable() > 0) {
        QByteArray localReadData = socket->readAll();
        if ( readData.isEmpty() )
            break;
        readData.append(localReadData);
    }
    return QString(readData);
}
