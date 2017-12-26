#include "item.h"

Item::Item(QWidget *parent) : QLabel(parent)
{
    this->setScaledContents(true);
    this->setFixedSize(fixedSize, fixedSize);
}

void Item::init(int id, QString type, QString imagePath)
{
    this->id = id;
    this->type = type;
    this->setToolTip(type);
    this->imagePath = imagePath;
    this->setPixmap(QPixmap(imagePath));
}

void Item::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    QPixmap itemPixmap = this->pixmap()->scaled(fixedSize, fixedSize);

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << id << type << imagePath << event->globalPos();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-draggable", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(itemPixmap);
    drag->setHotSpot(event->pos());
    drag->exec(Qt::CopyAction);
}

int Item::getID()
{
    return id;
}

QString Item::getType()
{
    return type;
}

QString Item::getImagePath()
{
    return imagePath;
}
