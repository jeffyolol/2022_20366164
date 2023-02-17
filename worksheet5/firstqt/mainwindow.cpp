#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
void MainWindow::handleButton(){
    emit statusUpdateMessage( QString("Add button was clicked"), 0);
    //QMessageBox msgBox;
    //msgBox.setText("add button was clicked");
    //msgBox.exec();
}

void MainWindow::handleButton_2(){
    emit statusUpdateMessage( QString("Add button 2 was clicked"), 0);
    //QMessageBox msgBox;
    //msgBox.setText("add button 2 was clicked");
    //msgBox.exec();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( ui-> pushButton, &QPushButton::released, this, &MainWindow::handleButton );

    connect( ui-> pushButton_2, &QPushButton::released, this, &MainWindow::handleButton_2 );

    connect( this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);

    this->partList = new ModelPartList("PartsList");

    ui->treeView->setModel(this->partList);

    ModelPart *rootItem = this->partList->getRootItem();

    for (int i = 0; i < 3; i++){

        QString name = QString("TopLevel %1").arg(i);

        QString visible("true");

        ModelPart *childItem = new ModelPart({ name, visible});

        rootItem-> appendChild(childItem);

        for (int j = 0; j < 5; j++){

            QString name = QString("Item %1,%2").arg(i).arg(j);

            QString visible("true");

            ModelPart *childChildItem = new ModelPart({name, visible });

            childItem->appendChild(childChildItem);
        }
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

