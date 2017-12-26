#include "menuwidget.h"
#include "ui_menuwidget.h"

MenuWidget::MenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    this->setMaximumHeight(this->getDesirableHeight());

    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
    connect(ui->exitButton,  SIGNAL(clicked()), this, SLOT(onExitButtonClicked()));
    connect(ui->hostButton, SIGNAL(clicked()), this, SLOT(onHostButtonClicked()));
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(onConnectButtonClicked()));

}

MenuWidget::~MenuWidget()
{
    delete ui;
}

int MenuWidget::getDesirableHeight()
{
    return fixedHeight;
}

void MenuWidget::onStartButtonClicked()
{
    emit startButtonClicked();
}

void MenuWidget::onExitButtonClicked()
{
    emit exitButtonClicked();
}

void MenuWidget::onHostButtonClicked()
{
    emit hostButtonClicked();
}

void MenuWidget::onConnectButtonClicked()
{
    emit connectButtonClicked();
}