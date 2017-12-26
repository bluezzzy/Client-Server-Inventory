#ifndef INVENTORY_H
#define INVENTORY_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QDrag>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <QDebug>

#include "item.h"

class Inventory : public QTableWidget
{
    Q_OBJECT
public:
    explicit Inventory(QWidget *parent = 0);
    QSize getSize();
    int getSectionsAmount();
    void inventoryDrop(QPoint destinationCoordinates, QDataStream &dataStream);
    QByteArray serializeData();

public slots:
    void        deserializeData(QByteArray data);

signals:
    void        innerDrag(int sourceIndex, int destinationIndex, int id, int amount);
    void        outerDrag(int destinationIndex, int id, int amount);
    void        cellDecrement(int destinationIndex, int amount);
    void        inventoryChanged();
    void        updateDB(int, int, int);

private:
    const int   size = 3;
    const int   sectionSize = 64;
    int         coordinatesToIndex(int row, int column);
    
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // INVENTORY_H