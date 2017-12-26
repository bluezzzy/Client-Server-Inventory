#ifndef CLIENT_H
#define CLIENT_H

#include <QAbstractSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDataStream>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    ~Client();
    bool connectToServer(int port);
    bool connected();
    void disconnect();

signals:
    void dataReceived(QByteArray);
    void disconnectedFromServer();

public slots:
    void onConnect();
    void onServerReady();
    void writeData(QByteArray);
    void serverGone();
    
private:
    QTcpSocket *tcpSocket;
    bool connectionStatus;
    qint32 readHeader(QByteArray source);
    QByteArray writeHeader(qint32 source);
};

#endif // CLIENT_H