#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPropertyAnimation>
#include <QCoreApplication>
#include <QMediaPlayer>
#include <QFile>
#include <QParallelAnimationGroup>

#include "item.h"
#include "dbmanager.h"

#include "server.h"
#include "client.h"

// UNCOMMENT THE LINE BELOW TO USE LOCAL DB
//#define LOCAL_DB

#ifdef LOCAL_DB
#define db_location "inventory_database.sqlite3";
#else
#define db_location ":memory:";
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void            collapse();
    
signals:
    void            sendData(QByteArray);

public slots:
    void            newGame();
    void            exit();
    void            expand();

    void            onCellDecrement(int cellIndex, int amount);
    void            onInnerDrag(int sourceIndex, int destinationIndex, int id, int amount);
    void            onOuterDrag(int destinationIndex, int id, int amount);
    void            onUpdateDB(int, int, int);

    void            hostServer();
    void            connectToServer();

    void            gatherData();
    void            onServerDisconnected();

private:
    const QString dbName = db_location

    Ui::MainWindow  *ui;

    QMediaPlayer    *mediaPlayer;

    DBManager       *dbManager;

    Server          *server;
    Client          *client;

    void            enableMenuWidget(bool enable);
    void            enableSelf(bool enable);

    void            playSound();

    bool            networkMode;
};

#endif // MAINWINDOW_H
