#include "timedensitydatapicker.h"
#include <qwt_plot_curve.h>
#include "timedensitydata.h"


TimeDensityDataPicker::TimeDensityDataPicker(QwtPlotMarker *markerX_, QwtPlotMarker *markerY_, 
  const AnalyseDialog::SegmentCurveMap &curveset_, QwtPlotCanvas *c):QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPicker::PointSelection,QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOn, c),
        markerX(markerX_),
        markerY(markerY_),
        curveset(curveset_) {
}

QwtText TimeDensityDataPicker::trackerText(const QPoint &p) const {
  int minDist = std::numeric_limits< int >::max();
  double minX = 0;
  CTImageTreeItem::SegmentationValues minValues;
   // prevent Warning
  minValues.min = 0; minValues.max = 0; minValues.mean = 0; minValues.stddev = 0; minValues.sampleCount = 0; minValues.segment = 0;
  QwtDoublePoint pdv;
  QPoint pv;
  for(AnalyseDialog::SegmentCurveMap::const_iterator it = curveset.begin(); it != curveset.end(); ++it) {
    CTImageTreeItem::SegmentationValues values;
    const TimeDensityData &data = dynamic_cast<const TimeDensityData&>(it->second->data());
    for(unsigned i=0; i < data.size(); ++i) {
      pdv.setX( data.getTimeAndValues(i, values) );
      pdv.setY( values.mean );
      pv = transform(pdv);
      int dx = pv.x() - p.x();
      int dy = pv.y() - p.y();
      int dist = dx * dx + dy * dy;
      if (dist < minDist) {
	minDist = dist;
	minX = pdv.x();
	minValues = values;
      }
    }
  }
  if (minDist < 25) {
    const BinaryImageTreeItem *binseg = dynamic_cast<const BinaryImageTreeItem *>(minValues.segment);
    QString text( binseg->getName() );
    text += "\nTime:" + QString::number(minX) + " s";
    text += "\nMean:" + QString::number(minValues.mean) + " HU";
    text += "\nStdDev:" + QString::number(minValues.stddev) + " HU";
    text += "\nMin:" + QString::number(minValues.min) + " HU";
    text += "\nMax:" + QString::number(minValues.max) + " HU";
    text += "\n#Samples:" + QString::number(minValues.sampleCount);
    if (!markerX->isVisible() || markerX->xValue()!= minX || markerY->yValue()!=minValues.mean) {
      markerX->setXValue(minX);
      markerY->setYValue(minValues.mean);
      markerX->setVisible(true);
      markerY->setVisible(true);
      const_cast<TimeDensityDataPicker*>(this)->canvas()->replot();
    }
    return text;
  } else {
    if (markerX->isVisible()) {
      markerX->setVisible(false);
      markerY->setVisible(false);
      const_cast<TimeDensityDataPicker*>(this)->canvas()->replot();
    }
    return QwtText();
  }
}