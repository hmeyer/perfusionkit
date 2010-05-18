#include <QtGui>
#include "mainwindow.h"
 
#include "volumeprojectionwidget.h"
#include "multiplanarreformatwidget.h"
#include <QSplitter>


MainWindow::MainWindow() {
  volImageWidget = new VolumeProjectionWidget;
  mprWidget = new MultiPlanarReformatWidget;
  splitter = new QSplitter;
  splitter->addWidget( mprWidget );
  splitter->addWidget( volImageWidget );

  setCentralWidget( splitter );

  createActions();
  createMenus();
  
  setWindowTitle(tr("Volume Visualizer"));
  setMinimumSize(200, 200);
  resize(480, 320);  
}

MainWindow::~MainWindow() {
  delete splitter;
  delete mprWidget;
  delete volImageWidget;
}


void MainWindow::setImage(vtkImageData *image) {
  mprWidget->setImage( image );
  volImageWidget->setImage( image );
}

void MainWindow::createActions()
{
     openAct = new QAction(tr("&Open..."), this);
     openAct->setShortcuts(QKeySequence::Open);
     openAct->setStatusTip(tr("Open a dicom file"));
     connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
     
     exitAct = new QAction(tr("E&xit"), this);
     exitAct->setStatusTip(tr("Exit the application"));
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));


     setDistanceAct = new QAction(tr("D&istance"), this);
     setDistanceAct->setStatusTip(tr("Set Viewer Distance"));
     connect(setDistanceAct, SIGNAL(triggered()), this, SLOT(setDistance()));

     setEyeAngleAct = new QAction(tr("E&ye Angle"), this);
     setEyeAngleAct->setStatusTip(tr("Set Viewer 3D-Eye Angle"));
     connect(setEyeAngleAct, SIGNAL(triggered()), this, SLOT(setEyeAngle()));

     toggleStereoAct = new QAction(tr("T&oggle Stereo"), this);
     toggleStereoAct->setStatusTip(tr("Toggle Stereo View"));
     connect(toggleStereoAct, SIGNAL(triggered()), this, SLOT(toggleStereo()));
     
     aboutAct = new QAction(tr("&About"), this);
     aboutAct->setStatusTip(tr("Show the application's About box"));
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(setDistanceAct);
  viewMenu->addAction(setEyeAngleAct);
  viewMenu->addAction(toggleStereoAct);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void MainWindow::about() {
     QMessageBox::about(this, tr("About Volume Visualizer"),
             tr("App for Stereo-Demos "));
}

void MainWindow::open() {
}

void MainWindow::setDistance() {
  double dist = volImageWidget->getCameraDistance();
  bool ok;
  dist = QInputDialog::getDouble(this, tr("Camera Distance"),
    tr("New Camera Distance:"), dist, 1e-3, 1e5, 2, &ok);  
  if (ok) volImageWidget->setCameraDistance( dist );
}

void MainWindow::setEyeAngle() {
  double ang = volImageWidget->getEyeAngle();
  bool ok;
  ang = QInputDialog::getDouble(this, tr("Stereo Eye Angle"),
    tr("New Eye Angle:"), ang, 1e-3, 90, 2, &ok);  
  if (ok) volImageWidget->setEyeAngle( ang );
}

void MainWindow::toggleStereo() {
  volImageWidget->toggleStereo();
}
