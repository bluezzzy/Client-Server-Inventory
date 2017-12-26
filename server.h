#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QByteArray>
#include <QMessageBox>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    bool createServer(int port = 2323);
    bool created();
    void close();
signals:
    void dataReceived(QByteArray);
    void clientRequest();
public slots:
    void onNewConnection();
    void onDisconnect();
    void writeData(QByteArray data);
    void onClientReady();
private:
    QTcpServer *tcpServer;
    QTcpSocket *clientSocket;
    bool full;
    qint32 readHeader(QByteArray source);
    QByteArray writeHeader(qint32 source);
    bool isCreated;
};

#endif // SERVER_H