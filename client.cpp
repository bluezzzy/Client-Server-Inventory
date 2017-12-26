#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    connectionStatus = false;

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(onConnect()));
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(serverGone()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(onServerReady()));

}

Client::~Client()
{
    disconnect();
}

bool Client::connectToServer(int port)
{
    tcpSocket->connectToHost(QHostAddress::LocalHost, port);
    return tcpSocket->waitForConnected();
}

void Client::onConnect()
{
    connectionStatus = true;
}

void Client::onServerReady()
{
    QByteArray *buffer = new QByteArray;
    qint32 *s = new qint32(0);
    qint32 size = *s;

    while (tcpSocket->bytesAvailable() > 0)
    {
        buffer->append(tcpSocket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size))
        {
            if (size == 0 && buffer->size() >= 4)
            {
                size = readHeader(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size)
            {
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
                emit dataReceived(data);
            }
        }
    }
    delete buffer;
    delete s;
}

bool Client::connected()
{
    return connectionStatus;
}

void Client::writeData(QByteArray data)
{
    if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
        tcpSocket->write(writeHeader(data.size()));
        tcpSocket->write(data);
        tcpSocket->waitForBytesWritten();
    }
}

void Client::disconnect()
{
    tcpSocket->disconnectFromHost();
}

qint32 Client::readHeader(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

QByteArray Client::writeHeader(qint32 source)
{
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

void Client::serverGone()
{
    connectionStatus = false;
    emit disconnectedFromServer();
}