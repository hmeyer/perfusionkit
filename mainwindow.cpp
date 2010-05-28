#include "mainwindow.h"
#include <QtGui>
#include "dicomselectordialog.h"


MainWindow::MainWindow() {
  setupUi( this );
  treeView->setModel( &imageModel );
}

MainWindow::~MainWindow() {
}


void MainWindow::setImage(const vtkImageData *image) {
  vtkImageData *non_const_image = const_cast<vtkImageData*>( image );
  mprView->setImage( non_const_image );
  volumeView->setImage( non_const_image );
}

void MainWindow::on_actionAbout_triggered() {
     QMessageBox::about(this, tr("About Perfusionkit"),
             tr("App for Perfusion Analysis"));
}

void MainWindow::on_actionExit_triggered() {
  qApp->exit();
}

void MainWindow::setFiles(const QStringList &names) {
  DicomSelectorDialogPtr selectDialog( new DicomSelectorDialog( this ) );
  selectDialog->setFilesOrDirectories( names );
  loadDicomData( selectDialog );
}

void MainWindow::on_actionOpenFile_triggered() {
  QStringList fnames = QFileDialog::getOpenFileNames(
                         this,
                         tr("Select one or more files to open"),
                         ".",
                         "", 0, QFileDialog::ReadOnly|QFileDialog::HideNameFilterDetails);
			 QString a;
			 std::string x;
			 x = a.toStdString();
  setFiles( fnames );
}

void MainWindow::on_actionOpenDirectory_triggered() {
  QString fname = QFileDialog::getExistingDirectory(
                         this,
                         tr("Select directoy to open"),
                         ".",
                         QFileDialog::ShowDirsOnly|QFileDialog::ReadOnly);
  DicomSelectorDialogPtr selectDialog( new DicomSelectorDialog( this ) );
  selectDialog->setFileOrDirectory( fname );
  loadDicomData( selectDialog );
}

void MainWindow::loadDicomData(DicomSelectorDialogPtr dicomSelector) {
  dicomSelector->exec();
  DicomImageListModel::Pointer loadedImages = dicomSelector->getSelectedImageDataList();
  imageModel.appendDicomImageList( *loadedImages );
}


void MainWindow::on_actionStereoDistance_triggered() {
  double dist = volumeView->getCameraDistance();
  bool ok;
  dist = QInputDialog::getDouble(this, tr("Camera Distance"),
    tr("New Camera Distance:"), dist, 1e-3, 1e5, 2, &ok);  
  if (ok) volumeView->setCameraDistance( dist );
}

void MainWindow::on_actionStereoEyeAngle_triggered() {
  double ang = volumeView->getEyeAngle();
  bool ok;
  ang = QInputDialog::getDouble(this, tr("Stereo Eye Angle"),
    tr("New Eye Angle:"), ang, 1e-3, 90, 2, &ok);  
  if (ok) volumeView->setEyeAngle( ang );
}

void MainWindow::on_actionStereoAnaglyph_triggered() {
  actionStereoAnaglyph->setChecked(true);
  actionStereoInterlaced->setChecked(false);
  actionStereoOff->setChecked(false);
  volumeView->setStereoMode( VolumeProjectionWidget::Anaglyph );
}

void MainWindow::on_actionStereoInterlaced_triggered() {
  actionStereoAnaglyph->setChecked(false);
  actionStereoInterlaced->setChecked(true);
  actionStereoOff->setChecked(false);
  volumeView->setStereoMode( VolumeProjectionWidget::Interlaced );
}

void MainWindow::on_actionStereoOff_triggered() {
  actionStereoAnaglyph->setChecked(false);
  actionStereoInterlaced->setChecked(false);
  actionStereoOff->setChecked(true);
  volumeView->setStereoMode( VolumeProjectionWidget::Off );
}
void MainWindow::on_viewButton_clicked() {
  QItemSelectionModel *selectionModel = treeView->selectionModel();
  if (selectionModel == NULL) return;
  QModelIndexList selectedRows = selectionModel->selectedRows();
  if (selectedRows.size() != 1) return;
  on_treeView_doubleClicked( selectedRows[0] );
}
void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
  vtkImageData const *image = imageModel.getItem( index ).getVTKImage();
  setImage( image );
}
