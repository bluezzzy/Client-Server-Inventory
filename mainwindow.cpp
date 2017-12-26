#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->inventory->setFixedSize(ui->inventory->getSize());

    mediaPlayer = new QMediaPlayer();
    mediaPlayer->setMedia(QUrl("qrc:/sounds/appleBiteSound"));
    mediaPlayer->setVolume(100);

    bool dbExists = QFile::exists(dbName);
    dbManager = new DBManager(dbName);

    if (!dbExists) {
        dbManager->executeQuery("CREATE TABLE Items ("
                                "id INTEGER PRIMARY KEY ASC, "
                                "type TEXT NOT NULL, "
                                "image_path TEXT NOT NULL)");

        dbManager->executeQuery("CREATE TABLE Inventory ("
                                "cell_index INTEGER PRIMARY KEY ASC, "
                                "item_id INTEGER, "
                                "amount INTEGER,"
                                "FOREIGN KEY(item_id) REFERENCES Items(id))");

        dbManager->executeQuery("INSERT INTO Items (type, image_path) "
                                "VALUES ('apple', ':/images/appleImage')");

        dbManager->executeQuery("INSERT INTO Items (type, image_path) "
                                "VALUES ('pear', ':/images/pearImage')");

        for (int i = 0; i < ui->inventory->getSectionsAmount(); i++)
        {
            dbManager->executeQuery(QString("INSERT INTO Inventory (cell_index) "
                                            "VALUES(%1)").arg(i+1));
        }

    } else {
        dbManager->executeQuery(QString("DELETE FROM Inventory "));
    }

    enableSelf(false);

    int itemsAmount = dbManager->count(QString("Items"));

    QStringList idList = dbManager->get("id", "Items");
    QStringList items = dbManager->get("type", "Items");
    QStringList imagesPaths = dbManager->get("image_path", "Items");

    for (int i = 0; i < itemsAmount; i++)
    {
        Item *item = new Item(ui->groupBox);
        item->init(QString(idList.at(i)).toInt(), items.at(i), imagesPaths.at(i));
        ui->horizontalLayout->addWidget(item);
    }

    connect(ui->menuWidget, SIGNAL(startButtonClicked()), this, SLOT(newGame()));
    connect(ui->menuWidget, SIGNAL(exitButtonClicked()),  this, SLOT(exit()));
    connect(ui->menuButton, SIGNAL(clicked()), this, SLOT(expand()));

    connect(ui->inventory, SIGNAL(cellDecrement(int, int)), this, SLOT(onCellDecrement(int,int)));
    connect(ui->inventory, SIGNAL(innerDrag(int,int,int,int)), this, SLOT(onInnerDrag(int,int,int,int)));
    connect(ui->inventory, SIGNAL(outerDrag(int,int,int)), this, SLOT(onOuterDrag(int,int,int)));
    connect(ui->inventory, SIGNAL(updateDB(int,int,int)), this, SLOT(onUpdateDB(int,int,int)));

    server = new Server(this);
    client = new Client(this);
    networkMode = false;

    connect(ui->menuWidget, SIGNAL(hostButtonClicked()), this, SLOT(hostServer()));
    connect(ui->menuWidget, SIGNAL(connectButtonClicked()), this, SLOT(connectToServer()));

    connect(ui->inventory, SIGNAL(inventoryChanged()), this, SLOT(gatherData()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enableMenuWidget(bool enable)
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->menuWidget, "maximumHeight");

    if (enable) {
        animation->setStartValue(0);
        animation->setEndValue(ui->menuWidget->getDesirableHeight());
    } else {
        animation->setStartValue(ui->menuWidget->getDesirableHeight());
        animation->setEndValue(0);
    }

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::expand()
{
    enableMenuWidget(true);
    enableSelf(false);
}

void MainWindow::newGame()
{
    if (networkMode) {
        networkMode = false;
        if (server->created()) {
            server->close();

            disconnect(this, SIGNAL(sendData(QByteArray)), this->server, SLOT(writeData(QByteArray)));
            disconnect(this->server, SIGNAL(dataReceived(QByteArray)), ui->inventory, SLOT(deserializeData(QByteArray)));
            disconnect(this->server, SIGNAL(clientRequest()), this, SLOT(gatherData()));
        }
        if (client->connected()) {
                client->disconnect();
        }
    }
    collapse();
}

void MainWindow::collapse()
{
    enableMenuWidget(false);
    enableSelf(true);

    for (int i = 0; i < ui->inventory->getSectionsAmount(); i++)
    {
        dbManager->executeQuery(QString("INSERT INTO Inventory "
                                " item_id = NULL, "
                                "amount = NULL "
                                "WHERE cell_index = %1").arg(i+1));
    }

    int sections = ui->inventory->getSectionsAmount();

    for (int x = 0; x < sections; x++) {
        for (int y = 0; y < sections; y++) {
            ui->inventory->removeCellWidget(x, y);
        }
    }
}

void MainWindow::exit()
{
    QCoreApplication::quit();
}

void MainWindow::enableSelf(bool enable)
{
    ui->menuButton->setEnabled(enable);
    ui->groupBox->setEnabled(enable);
}

void MainWindow::playSound()
{
    mediaPlayer->stop();
    mediaPlayer->play();
}

void MainWindow::onCellDecrement(int index, int amount)
{
    playSound();
    if (amount != 0) {
        dbManager->executeQuery(QString("UPDATE Inventory "
                                "SET amount = %1 "
                                "WHERE cell_index = %2").arg(amount).arg(index));
    } else {
       dbManager->executeQuery(QString("UPDATE Inventory "
                                "SET item_id = NULL, "
                                "amount = NULL "
                                "WHERE cell_index = %1").arg(index));
    }
}

void MainWindow::onInnerDrag(int sourceIndex, int destinationIndex, int id, int amount)
{
    dbManager->executeQuery(QString("UPDATE Inventory "
                                    "SET item_id = %1, "
                                    "amount = %2 "
                                    "WHERE cell_index = %3").arg(id).arg(amount).arg(destinationIndex));
    dbManager->executeQuery(QString("UPDATE Inventory "
                            "SET item_id = NULL, "
                            "amount = NULL "
                            "WHERE cell_index = %1").arg(sourceIndex));
}

void MainWindow::onOuterDrag(int index, int id, int amount)
{
    dbManager->executeQuery(QString("UPDATE Inventory "
                                    "SET item_id = %1, "
                                    "amount = %2 "
                                    "WHERE cell_index = %3").arg(id).arg(amount).arg(index));
}

void MainWindow::onUpdateDB(int index, int id, int amount) {
    if (id >= 0 && amount >= 0) {
        dbManager->executeQuery(QString("UPDATE Inventory "
                                        "SET item_id = %1, "
                                        "amount = %2 "
                                        "WHERE cell_index = %3").arg(id).arg(amount).arg(index));
    } else {
        dbManager->executeQuery(QString("UPDATE Inventory "
                                "SET item_id = NULL, "
                                "amount = NULL "
                                "WHERE cell_index = %1").arg(index));
    }
}

void MainWindow::hostServer()
{
    if (client->connected()) {
        QMessageBox::critical(0, "Critical", "You are connected to server");
        return;
    }
    if (server->createServer()) {
        connect(this, SIGNAL(sendData(QByteArray)), this->server, SLOT(writeData(QByteArray)));
        connect(this->server, SIGNAL(dataReceived(QByteArray)), ui->inventory, SLOT(deserializeData(QByteArray)));
        connect(this->server, SIGNAL(clientRequest()), this, SLOT(gatherData()));

        collapse();
        networkMode = true;
    }
}

void MainWindow::connectToServer()
{
    if (server->created()) {
        QMessageBox::critical(0, "Critical", "You are hosting server");
        return;
    }

    if (!client->connectToServer(2323)) {
        QMessageBox::critical(0, "Critical", "Cannot connect");
        return;
    }

    connect(this, SIGNAL(sendData(QByteArray)), this->client, SLOT(writeData(QByteArray)));
    connect(this->client, SIGNAL(dataReceived(QByteArray)), ui->inventory, SLOT(deserializeData(QByteArray)));
    connect(this->client, SIGNAL(disconnectedFromServer()), this, SLOT(onServerDisconnected()));

    collapse();
    networkMode = true;

}

void MainWindow::gatherData()
{
    if (networkMode) {
        emit sendData(ui->inventory->serializeData());
    }
}

void MainWindow::onServerDisconnected()
{
    QMessageBox::critical(0, "Critical", "Disconnected from server");

    disconnect(this, SIGNAL(sendData(QByteArray)), this->client, SLOT(writeData(QByteArray)));
    disconnect(this->client, SIGNAL(dataReceived(QByteArray)), ui->inventory, SLOT(deserializeData(QByteArray)));
    disconnect(this->client, SIGNAL(disconnectedFromServer()), this, SLOT(onServerDisconnected()));

    networkMode = false;
    expand();
}
