#include "mainwindow.h"
#include <QtGui>
#include "dicomselectordialog.h"
#include <boost/assign.hpp>


const CTImageTreeItem::DicomTagListType MainWindow::CTModelHeaderFields = boost::assign::list_of
  (CTImageTreeItem::DicomTagType("Patient Name", "0010|0010"))
  (CTImageTreeItem::DicomTagType("#Slices",CTImageTreeItem::getNumberOfFramesTag()))
  (CTImageTreeItem::DicomTagType("AcquisitionDatetime","0008|002a"));

MainWindow::MainWindow():imageModel(CTModelHeaderFields) {
  setupUi( this );
  treeView->setModel( &imageModel );
  connect( treeView->selectionModel(), SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection &) ),
		    this, SLOT( treeViewSelectionChanged(const QItemSelection &, const QItemSelection &) ) );
  connect( treeView, SIGNAL( customContextMenuRequested(const QPoint &) ),
		    this, SLOT( treeViewContextMenu(const QPoint &) ) );
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
  dicomSelector->getSelectedImageDataList(imageModel);
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
void MainWindow::on_actionLoadAllSeries_triggered() {
  imageModel.loadAllImages();
}
void MainWindow::treeViewSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
  if (selected.size()) {
    vtkImageData const *image = dynamic_cast<CTImageTreeItem*>(&imageModel.getItem( selected.first().topLeft() ))->getVTKImage();
    setImage( image );
  } else setImage( NULL );
}
void MainWindow::treeViewContextMenu(const QPoint &pos) {
  QModelIndex idx = treeView->indexAt(pos);
  if (idx.isValid() && !idx.parent().isValid()) {
    QMenu cm;
    
    QSignalMapper delMapper;
    QAction* delAction = cm.addAction("&Delete");
    delMapper.setMapping(delAction, idx.row());
    connect( delAction, SIGNAL( triggered() ),
      &delMapper, SLOT( map()  ) );
    connect( &delMapper, SIGNAL( mapped(int) ),
      &imageModel, SLOT( removeRow(int)  ) );

    QSignalMapper addSegMapper;
    QAction* addSegAction = cm.addAction("&Add Segment");
    addSegMapper.setMapping(addSegAction, idx.row());
    connect( addSegAction, SIGNAL( triggered() ),
      &addSegMapper, SLOT( map()  ) );
    connect( &addSegMapper, SIGNAL( mapped(int) ),
      &imageModel, SLOT( addSegment(int) ) );

    cm.exec(treeView->mapToGlobal(pos));
  }
}
