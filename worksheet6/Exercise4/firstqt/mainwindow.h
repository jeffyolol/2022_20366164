#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ModelPart.h"
#include "ModelPartList.h"
#include "vtkNew.h"
#include "vtkRenderer.h"
#include "vtkGenericOpenGLRenderWindow.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
public:
    void updateRender();
    void updateRenderFromTree(const QModelIndex &index);

    Q_OBJECT
public slots:
    void handleButton();
    void handleButton_2();
    void handleTreeClicked();
    void on_actionOpen_File_triggered();
    
    




signals:
    void statusUpdateMessage( const QString & message, int timeout );
public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Ui::MainWindow *vtkWidget;
    ModelPart* selectedItem;
    ModelPartList*partList;
    ModelPart* root;
    vtkSmartPointer<vtkRenderer>renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
};
#endif // MAINWINDOW_H
