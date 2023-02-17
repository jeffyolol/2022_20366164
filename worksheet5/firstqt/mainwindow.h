#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ModelPart.h"
#include "ModelPartList.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{

    Q_OBJECT
public slots:
    void handleButton();
    void handleButton_2();
    void handleTreeClicked();

signals:
    void statusUpdateMessage( const QString & message, int timeout );
public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ModelPartList*partList;
};
#endif // MAINWINDOW_H
