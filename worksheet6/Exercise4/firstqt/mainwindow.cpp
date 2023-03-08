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
#include "vtkSTLReader.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "modelPart.h"
#include "modelPartList.h"
#include "vtkGenericOpenGLRenderWindow.h"


void MainWindow::handleButton() {
    emit statusUpdateMessage(QString("Add button was clicked"), 0);
    //QMessageBox msgBox;
    //msgBox.setText("add button was clicked");
    //msgBox.exec();
}

void MainWindow::handleButton_2() {
    emit statusUpdateMessage(QString("Add button 2 was clicked"), 0);
    OptionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        emit statusUpdateMessage(QString("Dialog accepted"), 0);
    }
    else {
        emit statusUpdateMessage(QString("Dialog rejected"), 0);
    }
    //QMessageBox msgBox;
    //msgBox.setText("add button 2 was clicked");
    //msgBox.exec();
}

void MainWindow::handleTreeClicked() {
    //emit statusUpdateMessage( QString("Add button 2 was clicked"), 0);
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
    QString text = selectedPart->data(0).toString();
    emit statusUpdateMessage(QString("The selected item is:") + text, 0);
}

void MainWindow::on_actionOpen_File_triggered() {
    emit statusUpdateMessage(QString("Open File action triggered"), 0);
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("OpenFile"),
        "C:\\",
        tr("STL Files(*.stl);;Text Files(*.txt)"));
    if (fileName == "") { 
        return; }
    //QModelIndex index = ui->treeView->currentIndex();
    QStringList parts = fileName.split("/");
    QString path = parts.at(parts.size() - 1);
    ModelPart *newItem = new ModelPart({ path, "true"});
    newItem->loadSTL(fileName);
    selectedItem->appendChild(newItem);

    ui->treeView->reset();
    


}

void MainWindow::updateRender() {
    renderer->RemoveAllViewProps();
    updateRenderFromTree(partList->index(0, 0, QModelIndex()));
    renderer->Render();

}

void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (index.isValid()) {
        ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
    }
    if (!partList->hasChildren(index) || (index.flags() & Qt::ItemNeverHasChildren)){
        return;
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
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
    //ModelPart* getRootItem = this->partList->getRootItem();
   //ui->treeView->addAction(ui->actionItem_Options);
    this->partList = new ModelPartList("PartsList");
    ui->treeView->setModel(this->partList);
    root =this->partList->getRootItem();
    ModelPart *defaultLayer = new ModelPart({ "Default Layer", "true" });
    root->appendChild(defaultLayer);
    /*This is added in the main window*/
    /*Link a render window with the Qt widget*/
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    //updateRender();
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

