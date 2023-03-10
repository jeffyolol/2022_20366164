#include "mainwindow.h"
#include "vtk_glew.h"
//#include "qopenglext.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include "OptionDialog.h"
#include "vtkActor.h"
#include "vtkopenVRRenderWindow.h"
#include "vtkopenVRRenderWindowInteractor.h"
#include "vtkopenVRRenderer.h"
#include "vtkOpenVRCamera.h"
#include "vtkNew.h"
#include "vtkRenderer.h"
#include <vtkPolyDataMapper.h>
#include "vtkProperty.h"
#include "vtkSmartPointer.h"
#include "vtkOpenVRRenderer.h"
#include "vtkCamera.h"
#include "vtkCylinderSource.h"
#include "vtkSTLReader.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "modelPart.h"
#include "modelPartList.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "VRRenderThread.h"
#include "vtkNamedColors.h"


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
void MainWindow::on_actionopenVR_triggered(){
    emit statusUpdateMessage(QString("Open VR action triggered"), 0);

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

VRRenderThread::VRRenderThread( QObject* parent ) {
    /* Initialise actor list */
    actors = vtkActorCollection::New();

    /* Initialise command variables */
    rotateX = 0.;
    rotateY = 0.;
    rotateZ = 0.;
}


/* Standard destructor - this is important here as the class will be destroyed when the user
 * stops the VR thread, and recreated when the user starts it again. If class variables are
 * not deallocated properly then there will be a memory leak, where the program's total memory
 * usage will increase for each start/stop thread cycle.
 */
VRRenderThread::~VRRenderThread() {

}


void VRRenderThread::addActorOffline( vtkActor* actor ) {

    /* Check to see if render thread is running */
    if (!this->isRunning()) {
        double* ac = actor->GetOrigin();

        /* I have found that these initial transforms will position the FS
         * car model in a sensible position but you can experiment
         */
        actor->RotateX(-90);
        actor->AddPosition(-ac[0]+0, -ac[1]-100, -ac[2]-200);

        actors->AddItem(actor);
    }
}



void VRRenderThread::issueCommand( int cmd, double value ) {

    /* Update class variables according to command */
    switch (cmd) {
        /* These are just a few basic examples */
        case END_RENDER:
            this->endRender = true;
            break;

        case ROTATE_X:
            this->rotateX = value;
            break;

        case ROTATE_Y:
            this->rotateY = value;
            break;

        case ROTATE_Z:
            this->rotateZ = value;
            break;
    }
}

/* This function runs in a separate thread. This means that the program
 * can fork into two separate execution paths. This thread is triggered by
 * calling VRRenderThread::start()
 */
void VRRenderThread::run() {
    /* You might want to edit the 3D model once VR has started, however VTK is not "thread safe".
     * This means if you try to edit the VR model from the GUI thread while the VR thread is
     * running, the program could become corrupted and crash. The solution is to get the VR thread
     * to edit the model. Any decision to change the VR model will come fromthe user via the GUI thread,
     * so there needs to be a mechanism to pass data from the GUi thread to the VR thread.
     */

    vtkNew<vtkNamedColors> colors;

    // Set the background color.
    std::array<unsigned char, 4> bkg{ {26, 51, 102, 255} };
    colors->SetColor("BkgColor", bkg.data());

    // The renderer generates the image
    // which is then displayed on the render window.
    // It can be thought of as a scene to which the actor is added
    renderer = vtkOpenVRRenderer::New();

    renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());

    /* Loop through list of actors provided and add to scene */
    vtkActor* a;
    actors->InitTraversal();
    while( (a = (vtkActor*)actors->GetNextActor() ) ) {
        renderer->AddActor(a);
    }

    /* The render window is the actual GUI window
     * that appears on the computer screen
     */
    window = vtkOpenVRRenderWindow::New();

    window->Initialize();
    window->AddRenderer(renderer);

    /* Create Open VR Camera */
    camera = vtkOpenVRCamera::New();
    renderer->SetActiveCamera(camera);

    /* The render window interactor captures mouse events
     * and will perform appropriate camera or actor manipulation
     * depending on the nature of the events.
     */
    interactor = vtkOpenVRRenderWindowInteractor::New();
    interactor->SetRenderWindow(window);
    interactor->Initialize();
    window->Render();


    /* Now start the VR - we will implement the command loop manually
     * so it can be interrupted to make modifications to the actors
     * (i.e. to implement animation)
     */
    endRender = false;
    t_last = std::chrono::steady_clock::now();

    while( !interactor->GetDone() && !this->endRender ) {
        interactor->DoOneEvent( window, renderer );

        /* Check to see if enough time has elapsed since last update
         * This looks overcomplicated (and it is, C++ loves to make things unecessarily complicated!) but
         * is really just checking if more than 20ms have elaspsed since the last animation step. The
         * complications comes from the fact that numbers representing time on computers don't usually have
         * standard second/millisecond units. Because everything is a class in C++, the converion from
         * computer units to seconds/milliseconds ends up looking like what you see below.
         *
         * My choice of 20ms is arbitrary, if this value is too small the animation calculations could begin to
         * interfere with the interator processes and make the simulation unresponsive. If it is too large
         * the animations will be jerky. Play with the value to see what works best.
         */
        if (std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::steady_clock::now() - t_last).count() > 20) {

            /* Do things that might need doing ... */
            vtkActorCollection* actorList = renderer->GetActors();
            vtkActor* a;

            /* X Rotation */
            actorList->InitTraversal();
            while ((a = (vtkActor*)actorList->GetNextActor())) {
                a->RotateX(rotateX);
            }

            /* Y Rotation */
            actorList->InitTraversal();
            while ((a = (vtkActor*)actorList->GetNextActor())) {
                a->RotateY(rotateY);
            }

            /* Z Rotation */
            actorList->InitTraversal();
            while ((a = (vtkActor*)actorList->GetNextActor())) {
                a->RotateZ(rotateZ);
            }

            /* Remember time now */
            t_last = std::chrono::steady_clock::now();
        }
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

