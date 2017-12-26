#ifndef ITEM_H
#define ITEM_H

#include <QLabel>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QMimeData>

class Item : public QLabel
{
    Q_OBJECT
public:
    explicit Item(QWidget *parent = 0);
    void init(int id, QString type, QString imagePath);
    int getID();
    QString getType();
    QString getImagePath();

private:
    const int   fixedSize = 64;
    int         id;
    QString     type;
    QString     imagePath;

protected:
    void        mousePressEvent(QMouseEvent *event) ;
};

#endif // ITEM_H
