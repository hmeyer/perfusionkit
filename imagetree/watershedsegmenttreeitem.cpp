#include "watershedsegmenttreeitem.h"
#include <QInputDialog>
#include <boost/foreach.hpp>




WatershedSegmentTreeItem *WatershedSegmentTreeItem::Generate(CTImageTreeItem * parent) {
  const QString watershedTitle(QObject::tr("Watershed"));
  bool ok;
  QString segName = QInputDialog::getText(NULL, watershedTitle,
				      QObject::tr("Segment name:"), QLineEdit::Normal,
				      QObject::tr("WatershedSegment"), &ok);
  if (!ok) return NULL;
  WatershedSegmentTreeItem *item =  new WatershedSegmentTreeItem(parent, segName);
  if (!item) return NULL;
  if (!item->setup()) {
    delete item;
    return NULL;
  }
  return item;
}
    
    
WatershedSegmentTreeItem::WatershedSegmentTreeItem(CTImageTreeItem * parent, 
						   const QString &name)
:BinaryImageTreeItem(parent, ImageType::Pointer(), name) {
  thresholdInside = ThresholdBin2LabFilterType::New();
  thresholdInside->SetLowerThreshold( BinaryPixelOn );
  thresholdInside->SetUpperThreshold( BinaryPixelOn );
  thresholdInside->SetInsideValue( 1 );
  thresholdInside->SetOutsideValue( 0 );
  
  thresholdOutside = ThresholdBin2LabFilterType::New();
  thresholdOutside->SetLowerThreshold( BinaryPixelOn );
  thresholdOutside->SetUpperThreshold( BinaryPixelOn );
  thresholdOutside->SetInsideValue( 2 );
  thresholdOutside->SetOutsideValue( 0 );
  
  adder = AddFilterType::New();
  adder->SetInput1( thresholdInside->GetOutput() );
  adder->SetInput2( thresholdOutside->GetOutput() );
  
  gaussGradMag = GaussGradMagFilterType::New();
  gaussGradMag->SetInput( parent->getITKImage() );
  gaussGradMag->SetSigma( 2.0 );
  
  watershed = WatershedFilterType::New();
  watershed->SetInput1(gaussGradMag->GetOutput());
  watershed->SetInput2(adder->GetOutput());
  
  thresholdLabel = ThresholdLab2BinFilterType::New();
  thresholdLabel->SetInput( watershed->GetOutput() );
  thresholdLabel->SetLowerThreshold( 1 );
  thresholdLabel->SetUpperThreshold( 1 );
  thresholdLabel->SetInsideValue( BinaryPixelOn );
  thresholdLabel->SetOutsideValue( BinaryPixelOff );
}

bool WatershedSegmentTreeItem::setup() {
  const QString watershedTitle(QObject::tr("Watershed"));
  inside = const_cast<BinaryImageTreeItem *>
    (parent()->userSelectSegment(watershedTitle, QObject::tr("Select inside Region")));
  if (!inside) return false;
  thresholdInside->SetInput( inside->getITKImage() );
  
  outside = const_cast<BinaryImageTreeItem *>
    (parent()->userSelectSegment(watershedTitle, QObject::tr("Select outside Region")));
  if (!outside) return false;
  thresholdOutside->SetInput( outside->getITKImage() );
  
  bool ok;
  double gaussSigma = QInputDialog::getDouble(NULL, watershedTitle,
    QObject::tr("Sigma value for Gaussian Smoothing:"), gaussGradMag->GetSigma(), 0.00, 10.0, 2, &ok);  
  if (!ok) return false;
  gaussGradMag->SetSigma( gaussSigma );
  update();
  return true;
}

#include "../../myLibs/itkbasics.h"

void WatershedSegmentTreeItem::update(void) {
  gaussGradMag->Update();
  GradMagImageType::Pointer gradMagImage = gaussGradMag->GetOutput();
  itkBasic::ImageSave( gradMagImage, "/home/hmeyer/Desktop/perf-data/out%04d.png",150,300);
  thresholdLabel->Update();
  ImageType::Pointer result = thresholdLabel->GetOutput();
  setITKImage( result );
}


TreeItem* WatershedSegmentTreeItem::clone(TreeItem* clonesParent) const
{
  return BinaryImageTreeItem::clone(clonesParent);
}