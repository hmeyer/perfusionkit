#include "analysedialog.h"

#include <boost/foreach.hpp>
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
  picker = new TimeDensityDataPicker(markerPickerX, markerPickerY, segments, plot->canvas());
  tableGamma->setModel( &segments );
  tableGamma->verticalHeader()->setVisible(false);
  tableGamma->resizeColumnsToContents();
  buttonArtery->setSegmentListModel( &segments );
  
  listPatlak->setModel( &segments );
  plotPatlak->setTitle(tr("Patlak Plot"));
  plotPatlak->setAxisTitle(QwtPlot::xBottom, tr("Int C(t) dt / C(t) [/HU]"));
  plotPatlak->setAxisTitle(QwtPlot::yLeft, tr("C(t) / B(t)"));
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
  int imageIndex = 0;
  SegmentationValues values; values.accuracy = SegmentationValues::SimpleAccuracy;
  for(ImageSet::const_iterator ii = images.begin(); ii != images.end(); ++ii) {
    const CTImageTreeItem *ct = *ii;
    double relTime = ct->getTime() - firstTime;
    times.push_back(relTime);
    BOOST_FOREACH( SegmentInfo &currentSegment, segments) {
      values.segment = currentSegment.getSegment();
      if (ct->getSegmentationValues( values )) {
	currentSegment.pushSample(relTime, values);
      } else {
	QMessageBox::warning(this,tr("Analyse Error"),tr("Could not apply Segment ") + currentSegment.getName() + tr(" on image #") + QString::number(imageIndex));
      }
    }
    ++imageIndex;
  }
  BOOST_FOREACH( SegmentInfo &currentSegment, segments) {
    currentSegment.attachSampleCurve(plot);
    currentSegment.attachGammaCurve(plot);
  }
  return QDialog::exec();
}

bool AnalyseDialog::CTImageTimeCompareFunctor::operator()(const argT &x, const argT &y) const {
  return (x->getTime() < y->getTime());
}

void AnalyseDialog::on_sliderStart_valueChanged(int val) {
  markerStart->setXValue(times[val]);
  QModelIndexList indexList = tableGamma->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentInfo &seg = segments.getSegment(indexList[0]);
    seg.setGammaStartIndex(val);
    recalculateGamma(seg);
  }
  plot->replot();
}
void AnalyseDialog::on_sliderEnd_valueChanged(int val) {
  markerEnd->setXValue(times[val]);
  QModelIndexList indexList = tableGamma->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentInfo &seg = segments.getSegment(indexList.at(0));
    seg.setGammaEndIndex(val);
    recalculateGamma(seg);
  }
  plot->replot();
}

void AnalyseDialog::on_tableGamma_clicked(const QModelIndex & index) {
  on_tableGamma_activated(index);
}

void AnalyseDialog::on_tableGamma_activated(const QModelIndex & index) {
  sliderStart->setEnabled(true);
  sliderEnd->setEnabled(true);
  checkEnableGamma->setEnabled(true);
  markerStart->setVisible(true);
  markerEnd->setVisible(true);
  const SegmentInfo &seg = segments.getSegment( index );
  sliderStart->setValue(seg.getGammaStartIndex());
  sliderEnd->setValue(seg.getGammaEndIndex());
  checkEnableGamma->setChecked(seg.isGammaEnabled());
  buttonArtery->setSelection(seg.getArterySegment());
}

void AnalyseDialog::on_checkEnableGamma_toggled() {
  QModelIndexList indexList = tableGamma->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentInfo &seg = segments.getSegment(indexList.at(0));
    if (checkEnableGamma->isChecked()) {
      seg.setEnableGamma(true);
    }
    else seg.setEnableGamma(false);
    recalculateGamma(seg);
  }
}


void AnalyseDialog::on_buttonArtery_selected(const SegmentInfo *segment) {
  QModelIndexList indexList = tableGamma->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    segments.setArterySegment(indexList.at(0), segment);
  }
}


void AnalyseDialog::recalculateGamma(SegmentInfo &seginfo) {
  seginfo.recalculateGamma();
  segments.refresh();
  plot->replot();
}

void AnalyseDialog::on_listPatlak_clicked(const QModelIndex & index) {
  on_listPatlak_activated(index);
}

void AnalyseDialog::on_listPatlak_activated(const QModelIndex & index) {
}

