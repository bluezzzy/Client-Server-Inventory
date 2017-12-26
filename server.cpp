#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    full = false;
    isCreated = false;
}

Server::~Server()
{

}

bool Server::createServer(int port)
{
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, port)) {
        QMessageBox::critical(0, "Critical", tcpServer->errorString());
        return false;
    }

    isCreated = true;
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    return true;
}

void Server::onNewConnection()
{
    QTcpSocket *input = tcpServer->nextPendingConnection();

    if (full) {
        input->disconnectFromHost();
        delete input;
        return;
    }

    full = true;

    clientSocket = input;

    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onClientReady()));
    emit clientRequest();
}

void Server::onDisconnect()
{
    clientSocket->close();
    clientSocket->deleteLater();
    full = false;
}

void Server::close()
{
    tcpServer->close();
}

void Server::writeData(QByteArray data)
{
    if(full) {
        clientSocket->write(writeHeader(data.size()));
        clientSocket->write(data);
        clientSocket->waitForBytesWritten();
    }
}

void Server::onClientReady()
{
    QByteArray *buffer = new QByteArray;
    qint32 *s = new qint32(0);
    qint32 size = *s;
    while (clientSocket->bytesAvailable() > 0)
    {
        buffer->append(clientSocket->readAll());
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

qint32 Server::readHeader(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

QByteArray Server::writeHeader(qint32 source)
{
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

bool Server::created()
{
    return isCreated;
}