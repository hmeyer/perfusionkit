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

#include "clearancecalculator.h"
#include <limits>
#include <QMessageBox>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include "timedensitydata.h"
#include <binaryimagetreeitem.h>

double ClearanceCalculator::getLeftCTClearance() {
  return getCTClearance(parent->buttonLeftCortex, parent->buttonLeftArtery, parent->buttonLeftVein);
}
double ClearanceCalculator::getRightCTClearance() {
  return getCTClearance(parent->buttonRightCortex, parent->buttonRightArtery, parent->buttonRightVein);
}
bool ClearanceCalculator::checkSegment(SegmentSelectButton *seg) {
  if (seg->getSelectedSegment()!=NULL) {
    if (seg->getSelectedSegment()->isGammaEnabled()) return true;
    QMessageBox::warning(parent,QObject::tr("Clearance Error"),QObject::tr("Select a segment with Gamma Fit"));
    seg->setSelectedSegment(NULL);
  }
  return false;
}

double ClearanceCalculator::getCTClearance(SegmentSelectButton *cortexB, SegmentSelectButton *arteryB, SegmentSelectButton *veinB) {
  const SegmentInfo *cortex = cortexB->getSelectedSegment();
  const SegmentInfo *artery = arteryB->getSelectedSegment();
  const SegmentInfo *vein = veinB->getSelectedSegment();
  if (checkSegment(cortexB) && checkSegment(arteryB) 
    && vein!=NULL
    && parent->sliderClearanceStart->value() <= parent->sliderClearanceEnd->value()) {
      using namespace boost::accumulators;
      accumulator_set<double,features<tag::count, tag::mean> > arteryDensity;  
      accumulator_set<double,features<tag::count, tag::mean> > veinDensity;  
      for(int i = parent->sliderClearanceStart->value(); i <= parent->sliderClearanceEnd->value(); ++i) {
	arteryDensity( artery->getSampleData()->y(i) );
	veinDensity( vein->getSampleData()->y(i) );
      }
      double clearance = 60 * cortex->getSegment()->getVolumeInML()
	* cortex->getGammaMaxSlope() / artery->getGammaMaximum()
	* (mean(arteryDensity) - mean(veinDensity)) / (mean(arteryDensity)-artery->getGammaBaseline());
      return clearance;
  } else return std::numeric_limits< double >::quiet_NaN();
}
