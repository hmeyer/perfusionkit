#include "analysedialog.h"

#include "ctimagetreeitem.h"
#include "binaryimagetreeitem.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <vector>
#include <QMessageBox>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include "gammaVariate.h"
#include "timedensitydata.h"
#include "gammafitdata.h"
#include "timedensitydatapicker.h"






AnalyseDialog::AnalyseDialog(QWidget * parent, Qt::WindowFlags f)
  :QDialog( parent, f )
  ,markerStart(new QwtPlotMarker) 
  ,markerEnd(new QwtPlotMarker)
  ,markerPickerX(new QwtPlotMarker)
  ,markerPickerY(new QwtPlotMarker)
  ,grid(new QwtPlotGrid)
  ,segments(this) {
  setupUi( this );
  plot->setTitle(tr("Time Density Curves"));
  plot->setAxisTitle(QwtPlot::xBottom, tr("Time [s]"));
  plot->setAxisTitle(QwtPlot::yLeft, tr("Density [HU]"));
  plot->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
  
  markerStart->setLabel(tr("Start"));
  markerStart->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
  markerStart->setLineStyle(QwtPlotMarker::VLine);
  markerStart->setXValue(0);
  markerStart->setVisible(false);
  markerStart->attach(plot);  
  
  markerEnd->setLabel(tr("End"));
  markerEnd->setLabelAlignment(Qt::AlignLeft|Qt::AlignTop);
  markerEnd->setLineStyle(QwtPlotMarker::VLine);
  markerEnd->setXValue(0);
  markerEnd->setVisible(false);
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
  picker = new TimeDensityDataPicker(markerPickerX, markerPickerY, sampleCurveset, plot->canvas());
  listSegments->setModel( &segments );
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
      const BinaryImageTreeItem *seg = dynamic_cast<BinaryImageTreeItem*>(child);
      segments.addSegment(seg);
      boost::shared_ptr< QwtPlotCurve > curve( new QwtPlotCurve(seg->getName()) );
      sampleCurveset[seg] = curve;
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
  }
  sliderStart->setRange(0, images.size()-1 );
  sliderEnd->setRange(0, images.size()-1 );
}



int AnalyseDialog::exec(void ) {
  if (!images.size() || !segments.rowCount()) {
    QMessageBox::warning(this,tr("Analyse Error"),tr("Select at least one volume with at least one segment"));
    return QDialog::Rejected;
  }
  double firstTime = (*images.begin())->getTime();
  for(ImageSet::const_iterator ii = images.begin(); ii != images.end(); ++ii) {
    const CTImageTreeItem *ct = *ii;
    times.push_back(ct->getTime() - firstTime);
  }
  int imageIndex = 0;
  CTImageTreeItem::SegmentationValues values; values.accuracy = CTImageTreeItem::SimpleAccuracy;
  for(ImageSet::const_iterator ii = images.begin(); ii != images.end(); ++ii) {
    const CTImageTreeItem *ct = *ii;
    int segmentIndex = 0;
    for(SegmentListModel::iterator si = segments.begin(); si != segments.end(); ++si) {
      values.segment = si->segment;
      if (ct->getSegmentationValues( values )) {
	TimeDensityData &curveData = dynamic_cast<TimeDensityData &>(sampleCurveset[si->segment]->data());
	curveData.pushPoint(times[imageIndex], values);
      } else {
	QMessageBox::warning(this,tr("Analyse Error"),tr("Could not apply Segment ") + si->segment->getName() + tr(" on image #") + QString::number(imageIndex));
      }
      ++segmentIndex;
    }
    ++imageIndex;
  }
  return QDialog::exec();
}

bool AnalyseDialog::CTImageTimeCompareFunctor::operator()(const argT &x, const argT &y) const {
  return (x->getTime() < y->getTime());
}

void AnalyseDialog::on_sliderStart_valueChanged(int val) {
  markerStart->setXValue(times[val]);
  QModelIndexList indexList = listSegments->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentListModel::SegmentInfo &seg = segments.getSegment(indexList[0]);
    seg.gammaStartIndex = val;
    seg.gammaStart = times[val];
    recalculateGamma(seg);
  }
  plot->replot();
}
void AnalyseDialog::on_sliderEnd_valueChanged(int val) {
  markerEnd->setXValue(times[val]);
  QModelIndexList indexList = listSegments->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentListModel::SegmentInfo &seg = segments.getSegment(indexList.at(0));
    seg.gammaEndIndex = val;
    seg.gammaEnd = times[val];
    recalculateGamma(seg);
  }
  plot->replot();
}

void AnalyseDialog::on_listSegments_clicked(const QModelIndex & index) {
  on_listSegments_activated(index);
}

void AnalyseDialog::on_listSegments_activated(const QModelIndex & index) {
  sliderStart->setEnabled(true);
  sliderEnd->setEnabled(true);
  checkEnableGamma->setEnabled(true);
  markerStart->setVisible(true);
  markerEnd->setVisible(true);
  const SegmentListModel::SegmentInfo &seg = segments.getSegment( index );
  sliderStart->setValue(seg.gammaStartIndex);
  sliderEnd->setValue(seg.gammaEndIndex);
  checkEnableGamma->setChecked(seg.enableGammaFit);
}

void AnalyseDialog::on_checkEnableGamma_toggled() {
  QModelIndexList indexList = listSegments->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentListModel::SegmentInfo &seg = segments.getSegment(indexList.at(0));
    seg.enableGammaFit = checkEnableGamma->isChecked();
    recalculateGamma(seg);
  }
}




void AnalyseDialog::recalculateGamma(const SegmentListModel::SegmentInfo &seginfo) {
  const BinaryImageTreeItem *seg = seginfo.segment;
  CurvePtr &curve = gammaCurveset[ seg ];
  if (!seginfo.enableGammaFit || seginfo.gammaEndIndex < seginfo.gammaStartIndex + 2) {
    curve.reset();
  } else {
    if (!curve) {
	curve.reset( new QwtPlotCurve(seg->getName() + tr(" Gamma Fit")) );
	const RGBType &RGBColor = seg->getColor();
	QColor color = QColor( RGBColor[0], RGBColor[1], RGBColor[2] );
	QPen pen(color);
	pen.setStyle(Qt::DotLine);
	curve->setPen(pen);
	curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	curve->attach(plot);
	GammaFunctions::GammaVariate gv;
	GammaFitData curveData( gv, times.front(), times.back());
	curve->setData( curveData );
    }
    GammaFitData &gd = dynamic_cast<GammaFitData&>(curve->data());
    GammaFunctions::GammaVariate &gv = gd.getGammaVariate();
    gv.clearSamples();
    const QwtData &tdd = sampleCurveset[ seginfo.segment ]->data();
    for(unsigned i = seginfo.gammaStartIndex; i <= seginfo.gammaEndIndex; ++i) {
      gv.addSample(tdd.x(i), tdd.y(i));
    }
    gv.findFromSamples();
  }
  plot->replot();
}


