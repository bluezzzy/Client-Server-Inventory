#include "inventory.h"

Inventory::Inventory(QWidget *parent) : QTableWidget(parent)
{
    this->setColumnCount(this->size);
    this->setRowCount(this->size);

    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();

    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    this->setDragDropMode(DragDropMode::InternalMove);

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setSelectionMode(QAbstractItemView::NoSelection);
}

QSize Inventory::getSize()
{
    int fixedSize = size * sectionSize;
    return QSize(fixedSize, fixedSize);
}

int Inventory::getSectionsAmount()
{
    return size * size;
}


void Inventory::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-draggable")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Inventory::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-draggable")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Inventory::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-draggable")) {
        if(event->source() != this){
            QByteArray itemData = event->mimeData()->data("application/x-draggable");
            QDataStream dataStream(&itemData, QIODevice::ReadOnly);
            this->inventoryDrop(event->pos(), dataStream);
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void Inventory::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::RightButton) {
        return;
    }

    QPoint currentCell = event->pos();
    int currentRow = this->rowAt(currentCell.y());
    int currentColumn = this->columnAt(currentCell.x());

    QWidget *cell = this->cellWidget(currentRow, currentColumn);
    if (cell != NULL) {
        QLabel *label = qobject_cast<QLabel*>(cell->children().at(0)->children().at(0));
        int destinationIndex  = coordinatesToIndex(currentRow, currentColumn);
        int amount = label->text().toInt();
        if (amount >= 1) {
            if (amount > 1) {
                amount -= 1;
                label->setText(QString::number(amount));
            } else {
                amount = 0;
                this->removeCellWidget(currentRow, currentColumn);
            }
            emit cellDecrement(destinationIndex, amount);
        } else {
            return;
        }
    }
    emit inventoryChanged();
}

void Inventory::inventoryDrop(QPoint currentCell, QDataStream &dataStream)
{
    int id;
    QString type;
    QPoint sourceCell;
    QString imagePath;

    dataStream >> id >> type >> imagePath >> sourceCell;

    sourceCell = this->mapFromGlobal(sourceCell);

    int sourceRow = this->rowAt((sourceCell).y());
    int sourceColumn = this->columnAt(sourceCell.x());

    int currentRow = this->rowAt((currentCell).y());
    int currentColumn = this->columnAt(currentCell.x());

    if (sourceRow == currentRow && sourceColumn == currentColumn) {
        return;
    }

    QWidget *cell = new QWidget(this);
    Item *item = new Item(cell);
    item->init(id, type, imagePath);

    QLabel *label = new QLabel(item);

    label->setGeometry(0, 0, item->width() - label->fontInfo().pixelSize(), item->height());
    label->setAlignment(Qt::AlignBottom | Qt::AlignRight);

    int destinationIndex = coordinatesToIndex(currentRow, currentColumn);

    int amount = 0;

    if (sourceRow >= 0 && sourceColumn >= 0) {
        QLabel *oldLabel = qobject_cast<QLabel*>(this->cellWidget(sourceRow, sourceColumn)->children().at(0)->children().at(0));

        if (this->cellWidget(currentRow, currentColumn) == NULL) {
            amount = oldLabel->text().toInt();
            label->setText(QString::number(amount));
        } else {
            Item *newItem = qobject_cast<Item*>(this->cellWidget(currentRow, currentColumn)->children().at(0));
            if (newItem->getID() == id) {
                QLabel *newLabel = qobject_cast<QLabel*>(newItem->children().at(0));
                amount = newLabel->text().toInt() + oldLabel->text().toInt();
                label->setText(QString::number(amount));
            } else {
                return;
            }
        }
        int sourceIndex = coordinatesToIndex(sourceRow, sourceColumn);
        emit innerDrag(sourceIndex, destinationIndex, id, amount);
        this->removeCellWidget(sourceRow, sourceColumn);
    } else {
        if (this->cellWidget(currentRow, currentColumn) == NULL) {
            amount = 1;
            label->setText(QString::number(amount));
        } else {
            Item *newItem = qobject_cast<Item*>(this->cellWidget(currentRow, currentColumn)->children().at(0));
            if (newItem->getID() == item->getID()) {
                QLabel *newLabel = qobject_cast<QLabel*>(newItem->children().at(0));
                amount = newLabel->text().toInt() + 1;
                label->setText(QString::number(amount));
            } else {
                return;
            }
        }
        emit outerDrag(destinationIndex, id, amount);
    }
    this->setCellWidget(currentRow, currentColumn, cell);

    emit inventoryChanged();
}

int Inventory::coordinatesToIndex(int row, int column)
{
    int i = (column + 1) + row * size;
    return i;
}

QByteArray Inventory::serializeData()
{
    int id;
    int amount;

    QString type;
    QString imagePath;

    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (this->cellWidget(x, y) != NULL) {
                Item *item = qobject_cast<Item*>(this->cellWidget(x, y)->children().at(0));
                id = item->getID();
                type = item->getType();
                imagePath = item->getImagePath();
                QLabel *label = qobject_cast<QLabel*>(item->children().at(0));
                amount = label->text().toInt();
            } else {
                id = -1;
                amount = -1;
                imagePath = "nil";
                type = "nil";
            }
            dataStream << id << type << imagePath << amount;
        }
    }
    return data;
}

void Inventory::deserializeData(QByteArray data)
{
    QDataStream dataStream(&data, QIODevice::ReadOnly);

    int id;
    int amount;

    QString type;
    QString imagePath;

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            dataStream >> id >> type >> imagePath >> amount;
            if (id == -1) {
                this->removeCellWidget(x, y);
                emit updateDB(coordinatesToIndex(x, y), id, amount);
                continue;
            }

            QWidget *cell = new QWidget(this);
            Item *item = new Item(cell);
            item->init(id, type, imagePath);
            QLabel *label = new QLabel(item);
            label->setGeometry(0, 0, item->width() - label->fontInfo().pixelSize(), item->height());
            label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
            label->setText(QString::number(amount));
            this->setCellWidget(x, y, cell);
            emit updateDB(coordinatesToIndex(x, y), id, amount);
        }
    }
}