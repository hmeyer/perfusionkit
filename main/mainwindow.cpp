/*
    This file is part of perfusionkit.
    Copyright 2010 Henning Meyer

    perfusionkit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    perfusionkit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with perfusionkit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include <QtGui>
#include "dicomselectordialog.h"
#include "analysedialog.h"
#include "watershedsegmenttreeitem.h"
#include "binaryimagetreeitem.h"
#include <boost/assign.hpp>
#include <boost/foreach.hpp>

const DicomTagList MainWindow::CTModelHeaderFields = boost::assign::list_of
  (DicomTagType("Patient Name", "0010|0010"))
  (DicomTagType("#Slices",CTImageTreeItem::getNumberOfFramesTag()))
  (DicomTagType("AcquisitionDatetime","0008|002a"));

MainWindow::MainWindow():imageModel(CTModelHeaderFields),pendingAction(-1) {
  setupUi( this );
  treeView->setModel( &imageModel );
  connect( treeView, SIGNAL( customContextMenuRequested(const QPoint &) ),
    this, SLOT( treeViewContextMenu(const QPoint &) ) );
  connect( treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
	   this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
}

MainWindow::~MainWindow() {
}


void MainWindow::setImage(const CTImageTreeItem *imageItem) {
  vtkImageData *vtkImage = NULL;
  CTImageTreeItem::ConnectorHandle connectorPtr;
  if (imageItem) {
    connectorPtr = imageItem->getVTKConnector();
    vtkImage = connectorPtr->getVTKImageData();
  }
  if (connectorPtr != displayedCTImage) {
    while(!displayedSegments.empty()) {
      segmentHide( dynamic_cast<const BinaryImageTreeItem*>((*displayedSegments.begin())->getBaseItem()) );
    }
    mprView->setImage( vtkImage );
    volumeView->setImage( vtkImage );
    if (displayedCTImage && displayedCTImage->getBaseItem()) displayedCTImage->getBaseItem()->clearActiveDown();
    displayedCTImage = connectorPtr;
    if (displayedCTImage && displayedCTImage->getBaseItem()) displayedCTImage->getBaseItem()->setActive();
  }
}

void MainWindow::segmentShow( const BinaryImageTreeItem *segItem ) {
  if (segItem) {
    if (displayedCTImage && displayedCTImage->getBaseItem() != segItem->parent()) {
      setImage(dynamic_cast<const CTImageTreeItem*>(segItem->parent()));
    }
    ActionDispatch overlayAction(std::string("draw sphere on ") + segItem->getName().toAscii().data(), 
      boost::bind(&BinaryImageTreeItem::drawSphere, const_cast<BinaryImageTreeItem*>(segItem), 
        boost::bind( &QSpinBox::value, spinBoxSize ),
        _3, _4, _5,
        boost::bind( &QCheckBox::checkState, checkErase )
      ),
      ActionDispatch::ClickingAction, ActionDispatch::UnRestricted );
    BinaryImageTreeItem::ConnectorHandle segmentConnector = segItem->getVTKConnector();
    mprView->addBinaryOverlay( segmentConnector->getVTKImageData(), segItem->getColor(), overlayAction);
    displayedSegments.insert( segmentConnector );
    segItem->setActive();
  }
}

void MainWindow::segmentHide( const BinaryImageTreeItem *segItem ) {
  if (segItem) {
    clearPendingAction();
    DisplayedSegmentContainer::const_iterator it = displayedSegments.find( segItem->getVTKConnector() );
    if (it != displayedSegments.end()) {
      mprView->removeBinaryOverlay( (*it)->getVTKImageData() );
      displayedSegments.erase( it );
    }
    segItem->setActive(false);
  }
}

void MainWindow::on_buttonDraw_clicked() {
  BinaryImageTreeItem *seg = focusSegmentFromSelection();
  if (seg)
    mprView->activateOverlayAction(seg->getVTKConnector()->getVTKImageData());
}

void MainWindow::on_buttonThreshold_clicked() {
  BinaryImageTreeItem *seg = focusSegmentFromSelection();
  if (seg) {
    bool ok;
    double lower = QInputDialog::getDouble(this,tr("Threshold"), tr("Enter lower Threshold value"),
      -100, -4000, 4000, 1, &ok);
    if (!ok) return;
    double upper = QInputDialog::getDouble(this,tr("Threshold"), tr("Enter upper Threshold value"),
      100, -4000, 4000, 1, &ok);
    if (ok) {
      seg->thresholdParent(lower, upper);
      mprView->update();
    }
  }
}

void MainWindow::on_buttonRegionGrow_clicked() {
    BinaryImageTreeItem *seg = focusSegmentFromSelection();
    if (seg) {
      ActionDispatch regionGrowAction(std::string("click to region grow inside ") + seg->getName().toAscii().data(), 
        boost::bind(&BinaryImageTreeItem::regionGrow, seg, 
          _3, _4, _5,
          boost::function<void()>(boost::bind(&MainWindow::clearPendingAction, this))
        ),
        ActionDispatch::ClickingAction, ActionDispatch::UnRestricted );
      pendingAction = mprView->addAction(regionGrowAction);
      mprView->activateAction(pendingAction);
    }
}

void MainWindow::on_buttonDilate_clicked() {
  BinaryImageTreeItem *seg = focusSegmentFromSelection();
  if (seg) {
    bool ok;
    int iterations = QInputDialog::getInt(this,tr("Interations"), tr("Enter number of dilation iterations"),
      1, 1, 100, 1, &ok);
    if (!ok) return;
    seg->binaryDilate(iterations);
    mprView->update();
  }
}

void MainWindow::on_buttonErode_clicked() {
  BinaryImageTreeItem *seg = focusSegmentFromSelection();
  if (seg) {
    bool ok;
    int iterations = QInputDialog::getInt(this,tr("Interations"), tr("Enter number of erosion iterations"),
      1, 1, 100, 1, &ok);
    if (!ok) return;
    seg->binaryErode(iterations);
    mprView->update();
  }
}

void MainWindow::on_buttonAnalyse_clicked() {
  AnalyseDialog myDia(this);
  QModelIndexList selectedIndex = treeView->selectionModel()->selectedRows();
  for(QModelIndexList::Iterator index = selectedIndex.begin(); index != selectedIndex.end(); ++index) {
    if (index->isValid()) {
      TreeItem *item = &imageModel.getItem( *index );
      if (item->isA(typeid(CTImageTreeItem))) {
	myDia.addImage( dynamic_cast<CTImageTreeItem*>(item) );
      }
    }
  }
  std::list<TreeItem *> itemList;
  itemList.push_back( &imageModel.getItem(QModelIndex()) );
  while(!itemList.empty()) {
    TreeItem *currentItem = itemList.back();
    itemList.pop_back();
    int cnum = currentItem->childCount();
    for(int i = 0; i < cnum; i++ ) {
      itemList.push_back( &currentItem->child(i) );
    }
    if (currentItem->isA(typeid(BinaryImageTreeItem)))
      myDia.addSegment( dynamic_cast<BinaryImageTreeItem*>(currentItem) );
  }
  myDia.exec();
}

void MainWindow::on_buttonWatershed_clicked() {
  QModelIndexList selectedIndex = treeView->selectionModel()->selectedRows();
  if (selectedIndex.size() == 1 && selectedIndex[0].isValid()) {
    TreeItem *item = &imageModel.getItem( selectedIndex[0] );
    if (item->isA(typeid(CTImageTreeItem))) {
      CTImageTreeItem *ctitem = dynamic_cast<CTImageTreeItem*>(item);
      WatershedSegmentTreeItem *watershedItem = WatershedSegmentTreeItem::Generate(ctitem);
      if (watershedItem)
	ctitem->insertChild(watershedItem);
      return;
    }
  }
  QMessageBox::warning(this,tr("Watershed Error"),tr("Select one volume to segment."));
  return;
}



BinaryImageTreeItem *MainWindow::focusSegmentFromSelection(void) {
  clearPendingAction();
  QModelIndexList selectedIndex = treeView->selectionModel()->selectedRows();
  if (selectedIndex.size() != 1) {
    QMessageBox::warning(this,tr("Segment Error"),tr("Select one volume to edit"));
    return NULL;
  }
  if (selectedIndex[0].isValid()) {
    TreeItem *item = &imageModel.getItem( selectedIndex[0] );
    if (item->isA(typeid(CTImageTreeItem))) {
      CTImageTreeItem *ctitem = dynamic_cast<CTImageTreeItem*>(item);
      if (ctitem != displayedCTImage->getBaseItem())
	setImage( ctitem );
      if (ctitem->childCount() == 0) {
	item = ctitem->generateSegment();
      } else if (ctitem->childCount()==1) {
	item = &ctitem->child(0);
      } else {
	QMessageBox::warning(this,tr("Segment Error"),tr("Choose the segment to edit"));
	return  NULL;
      }
    }
    if (item->isA(typeid(BinaryImageTreeItem))) {
      BinaryImageTreeItem *seg = dynamic_cast<BinaryImageTreeItem*>(item);
      segmentShow(seg);
      return seg;
    }
  }
  return NULL;
}


void MainWindow::on_actionAbout_triggered() {
     QMessageBox::about(this, tr("About perfusionkit"),
             tr("App for Perfusion Analysis\n\n")+
                 
	      "perfusionkit is free software: you can redistribute it and/or modify"
	      "it under the terms of the GNU General Public License as published by"
	      "the Free Software Foundation, either version 3 of the License, or"
	      "(at your option) any later version.\n\n"

	      "perfusionkit is distributed in the hope that it will be useful,"
	      "but WITHOUT ANY WARRANTY; without even the implied warranty of"
	      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
	      "GNU General Public License for more details.\n\n"

	      "You should have received a copy of the GNU General Public License"
	      "along with perfusionkit.  If not, see <http://www.gnu.org/licenses/>.\n\n"

	      "Copyright 2010 Henning Meyer"


             );
}

void MainWindow::on_actionExit_triggered() {
  qApp->exit();
}

void MainWindow::setFiles(const QStringList &names) {
  if (!names.empty()) {
    DicomSelectorDialogPtr selectDialog( new DicomSelectorDialog( this ) );
    selectDialog->setFilesOrDirectories( names );
    loadDicomData( selectDialog );
  }
}

void MainWindow::on_actionOpenFile_triggered() {
  QStringList fnames = QFileDialog::getOpenFileNames(
    this,
    tr("Select one or more files to open"),
    ".",
    "", 0, QFileDialog::ReadOnly|QFileDialog::HideNameFilterDetails);
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

void MainWindow::on_actionCubicInterpolation_triggered() {
  mprView->setCubicInterpolation( actionCubicInterpolation->isChecked() );
  mprView->update();
}

void MainWindow::on_actionSaveProject_triggered() {
  QString pname( QString::fromStdString( imageModel.getSerializationPath() ) );
  if (pname.isEmpty()) pname = "./unnamed.perfproj";
  pname = QFileDialog::getSaveFileName( this,
    tr("Save Project"),
    pname,
    tr("Project Files (*.perfproj)"));
  if (!pname.isEmpty()) {
    imageModel.saveModelToFile(pname.toAscii().data());
  }
}

void MainWindow::on_actionOpenProject_triggered() {
  QString pname = QFileDialog::getOpenFileName( this,
    tr("Open Project"),
    "./unnamed.perfproj",
    tr("Project Files (*.perfproj)"));
  if (!pname.isEmpty()) {
    setImage(NULL);
    imageModel.openModelFromFile(pname.toAscii().data());
  }
}


void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
  if (index.isValid()) {
    TreeItem &item = imageModel.getItem( index );
    if (item.isA(typeid(CTImageTreeItem))) {
      if (displayedCTImage && &item == displayedCTImage->getBaseItem()) {
	setImage( NULL );
      } else {
	setImage( dynamic_cast<CTImageTreeItem*>(&item) );
      }
    } else if (item.isA(typeid(BinaryImageTreeItem))) {
      BinaryImageTreeItem *SegItem = dynamic_cast<BinaryImageTreeItem*>(&item);
      if (displayedSegments.find( SegItem->getVTKConnector() )==displayedSegments.end()) {
	segmentShow( SegItem );
      } else {
	segmentHide( SegItem );
      }
    }
  }
}

void MainWindow::removeSelectedImages() {
  QModelIndexList indexList = treeView->selectionModel()->selectedRows();
  BOOST_FOREACH( const QModelIndex &idx, indexList) {
    TreeItem &remitem = imageModel.getItem( idx );
    if (remitem.isA(typeid(CTImageTreeItem))) {
      CTImageTreeItem *remitemPtr = dynamic_cast<CTImageTreeItem*>(&remitem);
      if (displayedCTImage && displayedCTImage->getBaseItem() == remitemPtr) {
	setImage(NULL);
      }
    } else if (remitem.isA(typeid(BinaryImageTreeItem))) {
      BinaryImageTreeItem *remitemPtr = dynamic_cast<BinaryImageTreeItem*>(&remitem);
      segmentHide( remitemPtr );
    }
    imageModel.removeItem( idx );
  }
}

void MainWindow::createSegmentForSelectedImage() {
  QModelIndexList indexList = treeView->selectionModel()->selectedRows();
  if (indexList.count() == 1) {
    TreeItem &item = imageModel.getItem(indexList[0]);
    if (item.isA(typeid(CTImageTreeItem))) {
      dynamic_cast<CTImageTreeItem&>(item).generateSegment();
    }
  }
}

void MainWindow::changeColorForSelectedSegment() {
  QModelIndexList indexList = treeView->selectionModel()->selectedRows();
  if (indexList.count() == 1) {
    TreeItem &item = imageModel.getItem(indexList[0]);
    if (item.isA(typeid(BinaryImageTreeItem))) {
      BinaryImageTreeItem &binItem = dynamic_cast<BinaryImageTreeItem&>(item);
      QColor color = binItem.getColor();
      color = QColorDialog::getColor(color, this, tr("Choose new Segment Color for ") + binItem.getName());
      if (color.isValid())
	binItem.setColor(color);
    }
  }
}

void MainWindow::setupSelectedWatershedSegment() {
  QModelIndexList indexList = treeView->selectionModel()->selectedRows();
  if (indexList.count() == 1) {
    TreeItem &item = imageModel.getItem(indexList[0]);
    if (item.isA(typeid(WatershedSegmentTreeItem))) {
      WatershedSegmentTreeItem &wsitem = dynamic_cast<WatershedSegmentTreeItem&>(item);
      wsitem.setup();
      mprView->update();
    }
  }  
}

void MainWindow::updateSelectedWatershedSegment() {
  QModelIndexList indexList = treeView->selectionModel()->selectedRows();
  if (indexList.count() == 1) {
    TreeItem &item = imageModel.getItem(indexList[0]);
    if (item.isA(typeid(WatershedSegmentTreeItem))) {
      WatershedSegmentTreeItem &wsitem = dynamic_cast<WatershedSegmentTreeItem&>(item);
      wsitem.update();
      mprView->update();
    }
  }  
}



void MainWindow::treeViewContextMenu(const QPoint &pos) {
  QModelIndex idx = treeView->indexAt(pos);
  QModelIndexList indexList = treeView->selectionModel()->selectedRows();
  if (indexList.count()>0) {
    QMenu cm;
    if (indexList.count() == 1) {
      TreeItem &item = imageModel.getItem(indexList[0]);
      if (item.isA(typeid(CTImageTreeItem))) {
	QAction* addSegAction = cm.addAction("&Add Segment");
	connect( addSegAction, SIGNAL( triggered() ),
	  this, SLOT( createSegmentForSelectedImage())  );
      } else if (item.isA(typeid(BinaryImageTreeItem))) {
	QAction* addSegAction = cm.addAction("&Change Color");
	connect( addSegAction, SIGNAL( triggered() ),
	  this, SLOT( changeColorForSelectedSegment())  );
	if (item.isA(typeid(WatershedSegmentTreeItem))) {
	  QAction* setupAction = cm.addAction("&Setup");
	  connect( setupAction, SIGNAL( triggered() ),
	    this, SLOT( setupSelectedWatershedSegment())  );
	  QAction* updateAction = cm.addAction("&Update");
	  connect( updateAction, SIGNAL( triggered() ),
	    this, SLOT( updateSelectedWatershedSegment())  );
	}      
      }
    }
    QAction* addSegAction = cm.addAction("&Delete");
    connect( addSegAction, SIGNAL( triggered() ),
      this, SLOT( removeSelectedImages()  ) );
    cm.exec(treeView->mapToGlobal(pos));
  }
}

void MainWindow::clearPendingAction() {
  if (pendingAction != -1) {
    mprView->removeAction( pendingAction );
    pendingAction = -1;
  }
}

void MainWindow::on_actionMemoryUsage_triggered() {
  bool ok;
  int maxMemoryUsageInMB = QInputDialog::getInt(this, 
    tr("Memory Usage"), tr("maximum memory usage (approx.) [MB]:"), 
    imageModel.getMaxImageMemoryUsage() / (1024 * 1024), 256, 1024*100, 1, &ok);
  if (ok) {
    imageModel.setMaxImageMemoryUsage(maxMemoryUsageInMB * 1024 * 1024);
  }
}

void MainWindow::onSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
  int numSelected = treeView->selectionModel()->selectedRows().size();
  if (numSelected == 0) 
    statusbar->clearMessage();
  else
    statusbar->showMessage( QString::number( numSelected ) + tr(" item(s) selected") );
}

