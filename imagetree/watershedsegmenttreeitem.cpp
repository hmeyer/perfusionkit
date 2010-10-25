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
  initFilters();
  gaussGradMag->SetInput( parent->getITKImage() );
}

void WatershedSegmentTreeItem::initFilters(void) {
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
  gaussGradMag->SetSigma( 2.0 );
  
  watershed = WatershedFilterType::New();
  watershed->SetInput(gaussGradMag->GetOutput());
  watershed->SetMarkerImage(adder->GetOutput());
  
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

void WatershedSegmentTreeItem::update(void) {
  gaussGradMag->Update();
  thresholdLabel->Update();
  ImageType::Pointer result = thresholdLabel->GetOutput();
  setITKImage( result );
}


TreeItem* WatershedSegmentTreeItem::clone(TreeItem* clonesParent) const
{
  return BinaryImageTreeItem::clone(clonesParent);
}
