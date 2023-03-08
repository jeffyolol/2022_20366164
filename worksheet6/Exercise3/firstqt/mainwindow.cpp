#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include "OptionDialog.h"
#include "vtkNew.h"
#include "vtkRenderer.h"
#include <vtkPolyDataMapper.h>
#include "vtkProperty.h"
#include "vtkSmartPointer.h"
#include "vtkOpenVRRenderer.h"
#include "vtkCamera.h"
#include "vtkCylinderSource.h"
void MainWindow::handleButton(){
    emit statusUpdateMessage( QString("Add button was clicked"), 0);
    //QMessageBox msgBox;
    //msgBox.setText("add button was clicked");
    //msgBox.exec();
}

void MainWindow::handleButton_2(){
    emit statusUpdateMessage( QString("Add button 2 was clicked"), 0);
    OptionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted){
        emit statusUpdateMessage(QString("Dialog accepted"),0);
    } else{
        emit statusUpdateMessage(QString("Dialog rejected"), 0);
    }
    //QMessageBox msgBox;
    //msgBox.setText("add button 2 was clicked");
    //msgBox.exec();
}

void MainWindow::handleTreeClicked(){
    emit statusUpdateMessage( QString("Add button 2 was clicked"), 0);
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart *selectedPart = static_cast<ModelPart*>(index.internalPointer());
    QString text = selectedPart->data(0).toString();
    emit statusUpdateMessage(QString("The selected item is:")+text, 0);
}

void MainWindow::on_actionOpen_File_triggered(){
    emit statusUpdateMessage( QString("Open File action triggered"),0);
    QString filename =  QFileDialog::getOpenFileName ( this, tr("OpenFile"),
                                                       "C:\\",
                                                       tr("STL Files(*.stl);;Text Files(*.txt)"));
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( ui-> pushButton, &QPushButton::released, this, &MainWindow::handleButton );

    connect( ui-> pushButton_2, &QPushButton::released, this, &MainWindow::handleButton_2 );

    connect( ui-> treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked );

    connect( this, &MainWindow::statusUpdateMessage, ui->statusbar, &QStatusBar::showMessage);

    /*This is added in the main window*/
    /*Link a render window with the Qt widget*/
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);

    /*Add a render*/
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkCylinderSource> cylinder;
    cylinder->SetResolution(8);

    vtkNew<vtkPolyDataMapper> cylinderMapper;
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    vtkNew<vtkActor>cylinderActor;
    cylinderActor->SetMapper(cylinderMapper);
    cylinderActor->GetProperty()->SetColor(1.,0., 0.35);
    cylinderActor->RotateX(30.0);
    cylinderActor->RotateY(-45.0);

    renderer->AddActor(cylinderActor);

    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->ResetCameraClippingRange();

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

