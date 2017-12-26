#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>

namespace Ui {
class MenuWidget;
}

class MenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MenuWidget(QWidget *parent = 0);
    ~MenuWidget();
    int getDesirableHeight();
    
signals:
    void startButtonClicked();
    void exitButtonClicked();
    void hostButtonClicked();
    void connectButtonClicked();

private:
    Ui::MenuWidget *ui;

    const int fixedHeight = 38;

private slots:
    void onStartButtonClicked();
    void onExitButtonClicked();
    void onHostButtonClicked();
    void onConnectButtonClicked();
};

#endif // MENUWIDGET_H