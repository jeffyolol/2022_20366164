#include "mainwindow.h"
#include "ModelPart.h"
#include "ModelPartList.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
