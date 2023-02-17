#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
void MainWindow::handleButton(){
    //emit statusUpdateMessage( QString("Add button was clicked"), 0);
    QMessageBox msgBox;
    msgBox.setText("add button was clicked");
    msgBox.exec();
}

void MainWindow::handleButton_2(){
    //emit statusUpdateMessage( QString("Add button was clicked"), 0);
    QMessageBox msgBox;
    msgBox.setText("add button 2 was clicked");
    msgBox.exec();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( ui-> pushButton, &QPushButton::released, this, &MainWindow::handleButton );
    connect( ui-> pushButton_2, &QPushButton::released, this, &MainWindow::handleButton_2 );

}

MainWindow::~MainWindow()
{
    delete ui;
}

