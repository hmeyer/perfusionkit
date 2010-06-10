#include "mainwindow.h"
#include <QtGui>
#include "dicomselectordialog.h"
#include "binaryimagetreeitem.h"
#include <boost/assign.hpp>


const CTImageTreeItem::DicomTagList MainWindow::CTModelHeaderFields = boost::assign::list_of
  (CTImageTreeItem::DicomTagType("Patient Name", "0010|0010"))
  (CTImageTreeItem::DicomTagType("#Slices",CTImageTreeItem::getNumberOfFramesTag()))
  (CTImageTreeItem::DicomTagType("AcquisitionDatetime","0008|002a"));

MainWindow::MainWindow():imageModel(CTModelHeaderFields),selectedCTImage(NULL) {
  setupUi( this );
  treeView->setModel( &imageModel );
  connect( treeView, SIGNAL( customContextMenuRequested(const QPoint &) ),
		    this, SLOT( treeViewContextMenu(const QPoint &) ) );
}

MainWindow::~MainWindow() {
}


void MainWindow::setImage(VTKTreeItem *imageItem) {
  vtkImageData *vtkImage = NULL;
  if (imageItem != NULL)
    vtkImage = imageItem->getVTKImage();
  if (imageItem != selectedCTImage) {
    selectedSegments.clear();
    // TODO: clear mprViews Segements
    
    mprView->setImage( vtkImage );
    volumeView->setImage( vtkImage );
    if (selectedCTImage != NULL) selectedCTImage->clearActiveDown();
    selectedCTImage = imageItem;
    if (selectedCTImage != NULL) selectedCTImage->setActive();
  }
}

void MainWindow::segmentShow( BinaryImageTreeItem *segItem ) {
  if (segItem) {
    mprView->addBinaryOverlay( segItem->getVTKImage(), segItem->getColor() );
    selectedSegments.insert( segItem );
    segItem->setActive();
  }
}

void MainWindow::segmentHide( BinaryImageTreeItem *segItem ) {
  if (segItem) {
    mprView->removeBinaryOverlay( segItem->getVTKImage() );
    selectedSegments.erase( segItem );
    segItem->setActive(false);
  }
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



void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
  if (index.isValid()) {
    TreeItem &item = imageModel.getItem( index );
    VTKTreeItem *CTItem = NULL;
    BinaryImageTreeItem *SegItem = NULL;
    try {
      if (item.depth() == 1) {
	CTItem = dynamic_cast<VTKTreeItem*>(&item);
      } else if (item.depth() == 2) {
	SegItem = dynamic_cast<BinaryImageTreeItem*>(&item);
	CTItem = dynamic_cast<VTKTreeItem*>(item.parent());
      }
      if (CTItem != selectedCTImage) {
	if (CTItem) {
	  if (selectedCTImage) selectedCTImage->setActive(false);
	  setImage( CTItem );
	  selectedCTImage = CTItem;
	  selectedCTImage->setActive();
	} else setImage( NULL );
      }
      if (SegItem) {
	if (selectedSegments.find( SegItem )==selectedSegments.end()) {
	  segmentShow( SegItem );
	} else {
	  segmentHide( SegItem );
	}
      }
    } catch(...) {
      selectedSegments.clear();
      setImage( NULL );
    }
  }
}

void MainWindow::removeCTImage(int number) {
  TreeItem &remitem = imageModel.getItem( imageModel.index( number, 0 ) );
  VTKTreeItem *remitemPtr = dynamic_cast<VTKTreeItem*>(&remitem);
  if (selectedCTImage == remitemPtr) {
    setImage(NULL);
  }
  imageModel.removeCTImage(number);
}

void MainWindow::treeViewContextMenu(const QPoint &pos) {
  QModelIndex idx = treeView->indexAt(pos);
  if (idx.isValid()) {
    TreeItem &item = imageModel.getItem(idx);
    if ( item.depth() == 1 ) {
      QMenu cm;
      
      QSignalMapper delMapper;
      QAction* delAction = cm.addAction("&Delete");
      delMapper.setMapping(delAction, idx.row());
      connect( delAction, SIGNAL( triggered() ),
	&delMapper, SLOT( map()  ) );
      connect( &delMapper, SIGNAL( mapped(int) ),
	this, SLOT( removeCTImage(int)  ) );

      QSignalMapper addSegMapper;
      QAction* addSegAction = cm.addAction("&Add Segment");
      addSegMapper.setMapping(addSegAction, idx.row());
      connect( addSegAction, SIGNAL( triggered() ),
	&addSegMapper, SLOT( map()  ) );
      connect( &addSegMapper, SIGNAL( mapped(int) ),
	&imageModel, SLOT( createSegment(int) ) );

      cm.exec(treeView->mapToGlobal(pos));
    }
  }
}
