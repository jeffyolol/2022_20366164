#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
void MainWindow::handleButton(){
    QMessageBox msgBox;
    msgBox.setText("add button was clicked");
    msgBox.exec();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( ui-> pushButton, &QPushButton::released, this, &MainWindow::handleButton );
}

MainWindow::~MainWindow()
{
    delete ui;
}

