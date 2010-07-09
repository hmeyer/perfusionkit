#include "analysedialog.h"

#include "ctimagetreeitem.h"
#include "binaryimagetreeitem.h"
#include <qwt_data.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <vector>
#include <QMessageBox>

AnalyseDialog::AnalyseDialog(QWidget * parent, Qt::WindowFlags f)
  :QDialog( parent, f )
  ,markerStart(new QwtPlotMarker) 
  ,markerEnd(new QwtPlotMarker)
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
  
  grid->enableX(true); grid->enableX(false);
  grid->attach(plot);
  
  sliderStart->setTracking(true);
  sliderEnd->setTracking(true);
}

AnalyseDialog::~AnalyseDialog() {
  delete grid;
  delete markerStart;
  delete markerEnd;
}


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
        return density[i];
    }
    void pushPoint( double t, double d) {
      if (time.size() == 0 || t> time[time.size()-1]) {
	time.push_back(t);
	density.push_back(d);
      }
    }
private:
  std::vector< double > time;
  std::vector< double > density;
};

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
  std::vector< boost::shared_ptr< QwtPlotCurve > > curveset;
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
    int i = 0;
    for(ImageSet::const_iterator ii = images.begin(); ii != images.end(); ++ii) {
      const CTImageTreeItem *ct = *ii;
      double mean, stddev;
      int min, max;
      if (ct->getSegmentationValues(seg, mean, stddev, min, max)) {
	curveData.pushPoint(times[i], mean);
      } else {
	QMessageBox::warning(this,tr("Analyse Error"),tr("Could not apply Segment ") + seg->getName() + tr(" on image #") + QString::number(i));
      }
      ++i;
    }
    curve->setData( curveData );
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

