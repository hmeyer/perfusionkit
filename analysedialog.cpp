#include "analysedialog.h"

#include "ctimagetreeitem.h"
#include "binaryimagetreeitem.h"
#include <qwt_data.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_picker.h>
#include <vector>
#include <QMessageBox>


class TimeDensityData: public QwtData {
public:
    virtual QwtData *copy() const {
        return new TimeDensityData(*this);
    }
    virtual size_t size() const {
        return time.size();
    }
    virtual double x(size_t i) const {
        return time[i];
    }
    virtual double y(size_t i) const {
        return values[i].mean;
    }
    double getTimeAndValues( size_t i, CTImageTreeItem::SegmentationValues &v ) const {
      v = values[i];
      return time[i];
    }
    void pushPoint( double t, const CTImageTreeItem::SegmentationValues &v) {
      if (time.size() == 0 || t> time[time.size()-1]) {
	time.push_back(t);
	values.push_back(v);
      }
    }
private:
  std::vector< double > time;
  std::vector< CTImageTreeItem::SegmentationValues > values;
};


class TimeDensityDataPicker: public QwtPlotPicker {
  public:
  TimeDensityDataPicker(QwtPlotMarker *markerX_, QwtPlotMarker *markerY_, const AnalyseDialog::CurveList &curveset_, QwtPlotCanvas *);
  virtual QwtText trackerText(const QPoint &) const;
  protected:
    QwtPlotMarker *markerX;
    QwtPlotMarker *markerY;
    const AnalyseDialog::CurveList &curveset;
};


AnalyseDialog::AnalyseDialog(QWidget * parent, Qt::WindowFlags f)
  :QDialog( parent, f )
  ,markerStart(new QwtPlotMarker) 
  ,markerEnd(new QwtPlotMarker)
  ,markerPickerX(new QwtPlotMarker)
  ,markerPickerY(new QwtPlotMarker)
  ,grid(new QwtPlotGrid) {
  setupUi( this );
  plot->setTitle(tr("Time Density Curves"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Time [s]"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Density [HU]"));
  
  markerStart->setLabel(tr("Start"));
  markerStart->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
  markerStart->setLineStyle(QwtPlotMarker::VLine);
  markerStart->setXValue(0);
  markerStart->attach(plot);  
  
  markerEnd->setLabel(tr("End"));
  markerEnd->setLabelAlignment(Qt::AlignLeft|Qt::AlignTop);
  markerEnd->setLineStyle(QwtPlotMarker::VLine);
  markerEnd->setXValue(0);
  markerEnd->attach(plot);  
  
  markerPickerX->setLineStyle(QwtPlotMarker::VLine);
  markerPickerY->setLineStyle(QwtPlotMarker::HLine);
  markerPickerX->setLinePen(QPen(Qt::red));
  markerPickerY->setLinePen(QPen(Qt::red));
  markerPickerX->setVisible(false);
  markerPickerY->setVisible(false);
  markerPickerX->attach(plot);
  markerPickerY->attach(plot);

  
  grid->enableX(true); grid->enableX(false);
  grid->attach(plot);
  
  sliderStart->setTracking(true);
  sliderEnd->setTracking(true);
  picker = new TimeDensityDataPicker(markerPickerX, markerPickerY, curveset, plot->canvas());
}

AnalyseDialog::~AnalyseDialog() {
  delete markerPickerX;
  delete markerPickerY;
  delete picker;
  delete grid;
  delete markerStart;
  delete markerEnd;
}

void AnalyseDialog::addImage(CTImageTreeItem *image) {
  images.insert(image);
  int childNum = image->childCount();
  for(int i = 0; i < childNum; ++i) {
    TreeItem *child = &image->child(i);
    if (typeid(*child) == typeid(BinaryImageTreeItem)) {
      segments.insert( dynamic_cast<BinaryImageTreeItem*>(child) );
    }
  }
}

int AnalyseDialog::exec(void ) {
  if (!images.size() || !segments.size()) {
    QMessageBox::warning(this,tr("Analyse Error"),tr("Select at least one volume with at least one segment"));
    return QDialog::Rejected;
  }
  double firstTime = (*images.begin())->getTime();
  for(ImageSet::const_iterator ii = images.begin(); ii != images.end(); ++ii) {
    const CTImageTreeItem *ct = *ii;
    times.push_back(ct->getTime() - firstTime);
  }
  curveset.clear();
  
  for(SegmentSet::const_iterator si = segments.begin(); si != segments.end(); ++si) {
    const BinaryImageTreeItem *seg = *si;
    boost::shared_ptr< QwtPlotCurve > curve( new QwtPlotCurve(seg->getName()) );
    curveset.push_back( curve );
    const RGBType &RGBColor = seg->getColor();
    QColor color = QColor( RGBColor[0], RGBColor[1], RGBColor[2] );
    curve->setPen(QPen(color));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(plot);
    QwtSymbol symbol; 
    symbol.setStyle( QwtSymbol::Ellipse );
    symbol.setSize(8);
    symbol.setPen(QPen(color));
    symbol.setBrush(QBrush(color.darker(130)));
    curve->setSymbol( symbol );
    TimeDensityData curveData;
    curve->setData( curveData );
  }
  int imageIndex = 0;
  CTImageTreeItem::SegmentationValues values; values.accuracy = CTImageTreeItem::SimpleAccuracy;
  for(ImageSet::const_iterator ii = images.begin(); ii != images.end(); ++ii) {
    const CTImageTreeItem *ct = *ii;
    int segmentIndex = 0;
    for(SegmentSet::const_iterator si = segments.begin(); si != segments.end(); ++si) {
      values.segment = *si;
      if (ct->getSegmentationValues( values )) {
	TimeDensityData &curveData = dynamic_cast<TimeDensityData &>(curveset[segmentIndex]->data());
	curveData.pushPoint(times[imageIndex], values);
      } else {
	QMessageBox::warning(this,tr("Analyse Error"),tr("Could not apply Segment ") + (*si)->getName() + tr(" on image #") + QString::number(imageIndex));
      }
      ++segmentIndex;
    }
    ++imageIndex;
  }
  
  sliderStart->setRange(0, times.size()-1 );
  sliderStart->setSliderPosition(0);
  sliderEnd->setRange(0, times.size()-1 );
  sliderEnd->setSliderPosition( times.size() -1 );
  plot->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
  return QDialog::exec();
}

double AnalyseDialog::stringToSec(const std::string &timeString) {
  return 0;
}

bool AnalyseDialog::CTImageTimeCompareFunctor::operator()(const argT &x, const argT &y) const {
  return (x->getTime() < y->getTime());
}

void AnalyseDialog::on_sliderStart_valueChanged(int val) {
  markerStart->setXValue(times[val]);
  plot->replot();
}
void AnalyseDialog::on_sliderEnd_valueChanged(int val) {
  markerEnd->setXValue(times[val]);
  plot->replot();
}



TimeDensityDataPicker::TimeDensityDataPicker(QwtPlotMarker *markerX_, QwtPlotMarker *markerY_, 
  const AnalyseDialog::CurveList &curveset_, QwtPlotCanvas *c):QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
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
  for(AnalyseDialog::CurveList::const_iterator it = curveset.begin(); it != curveset.end(); ++it) {
    CTImageTreeItem::SegmentationValues values;
    const TimeDensityData &data = dynamic_cast<const TimeDensityData&>((*it)->data());
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
    BinaryImageTreeItem *binseg = dynamic_cast<BinaryImageTreeItem *>(minValues.segment);
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