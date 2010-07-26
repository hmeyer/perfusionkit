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
#include "clearancecalculator.h"






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
  
  buttonLeftCortex->setSegmentListModel( &segments );
  buttonLeftArtery->setSegmentListModel( &segments );
  buttonLeftVein->setSegmentListModel( &segments );
  buttonRightCortex->setSegmentListModel( &segments );
  buttonRightArtery->setSegmentListModel( &segments );
  buttonRightVein->setSegmentListModel( &segments );
  
  connect(buttonLeftCortex,SIGNAL(selected(const SegmentInfo*)),this,SLOT(onClearanceParametersChanged()));
  connect(buttonLeftArtery,SIGNAL(selected(const SegmentInfo*)),this,SLOT(onClearanceParametersChanged()));
  connect(buttonLeftVein,SIGNAL(selected(const SegmentInfo*)),this,SLOT(onClearanceParametersChanged()));
  connect(buttonRightCortex,SIGNAL(selected(const SegmentInfo*)),this,SLOT(onClearanceParametersChanged()));
  connect(buttonRightArtery,SIGNAL(selected(const SegmentInfo*)),this,SLOT(onClearanceParametersChanged()));
  connect(buttonRightVein,SIGNAL(selected(const SegmentInfo*)),this,SLOT(onClearanceParametersChanged()));
  connect(sliderClearanceStart,SIGNAL(valueChanged(int)),this,SLOT(onSliderClearanceChanged()));
  connect(sliderClearanceEnd,SIGNAL(valueChanged(int)),this,SLOT(onSliderClearanceChanged()));
  
  listPatlak->setModel( &segments );
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
    currentSegment.attachSampleCurves(plot);
  }
  sliderClearanceStart->setMaximum(images.size()-1);
  sliderClearanceEnd->setMaximum(images.size()-1);
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
    recalculateData(seg);
  }
  plot->replot();
}
void AnalyseDialog::on_sliderEnd_valueChanged(int val) {
  markerEnd->setXValue(times[val]);
  QModelIndexList indexList = tableGamma->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentInfo &seg = segments.getSegment(indexList.at(0));
    seg.setGammaEndIndex(val);
    recalculateData(seg);
  }
  plot->replot();
}

void AnalyseDialog::on_tableGamma_clicked(const QModelIndex & index) {
  on_tableGamma_activated(index);
}

void AnalyseDialog::on_tableGamma_activated(const QModelIndex & index) {
  tableGamma->selectionModel()->select(index, QItemSelectionModel::Rows);
  sliderStart->setEnabled(true);
  sliderEnd->setEnabled(true);
  checkEnableGamma->setEnabled(true);
  markerStart->setVisible(true);
  markerEnd->setVisible(true);
  const SegmentInfo &seg = segments.getSegment( index );
  sliderStart->setValue(seg.getGammaStartIndex());
  sliderEnd->setValue(seg.getGammaEndIndex());
  checkEnableGamma->setChecked(seg.isGammaEnabled());
  buttonArtery->setSelectedSegment(seg.getArterySegment());
}

void AnalyseDialog::on_checkEnableGamma_toggled() {
  QModelIndexList indexList = tableGamma->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentInfo &seg = segments.getSegment(indexList.at(0));
    if (checkEnableGamma->isChecked()) {
      seg.setEnableGamma(true);
    }
    else seg.setEnableGamma(false);
    recalculateData(seg);
  }
}


void AnalyseDialog::on_buttonArtery_selected(const SegmentInfo *segment) {
  QModelIndexList indexList = tableGamma->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    segments.setArterySegment(indexList.at(0), segment);
  }
}


void AnalyseDialog::recalculateData(SegmentInfo &seginfo) {
  seginfo.recalculateGamma();
  segments.refresh();
  plot->replot();
  refreshPatlakData();
}

void AnalyseDialog::refreshPatlakData() {
  QModelIndexList indexList = listPatlak->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    labelBV->setText(QString::number(segments.getSegment(indexList[0]).getPatlakIntercept()));
    labelClearance->setText(QString::number(60 * segments.getSegment(indexList[0]).getPatlakSlope()));
  } else {
    labelBV->setText(QString());
    labelClearance->setText(QString());
  }
  plotPatlak->replot();
  labelBV->repaint();
  labelClearance->repaint();
}

void AnalyseDialog::on_listPatlak_clicked(const QModelIndex & index) {
  on_listPatlak_activated(index);
}

void AnalyseDialog::on_listPatlak_activated(const QModelIndex & index) {
  listPatlak->selectionModel()->select(index, QItemSelectionModel::Rows);
  SegmentInfo &seg = segments.getSegment( index );
  BOOST_FOREACH( SegmentInfo &currentSegment, segments) {
    currentSegment.detachPatlak();
  }
  if (seg.attachPatlak( plotPatlak ) == false) {
    QMessageBox::warning(this,tr("Analyse Error"),tr("You need to define an artery Segment and fit Gamma Functions to ") + seg.getName() + tr(" and its artery Segment"));
    plotPatlak->replot();
  } else {
    refreshPatlakData();
    sliderPatlakStart->setEnabled(true);
    sliderPatlakEnd->setEnabled(true);
    sliderPatlakStart->setValue(seg.getPatlakStartIndex());
    sliderPatlakEnd->setValue(seg.getPatlakEndIndex());
    sliderPatlakStart->setMaximum(images.size()-2);
    sliderPatlakEnd->setMaximum(images.size()-2);
  }
}

void AnalyseDialog::on_sliderPatlakStart_valueChanged(int val) {
  markerStart->setXValue(times[val]);
  QModelIndexList indexList = listPatlak->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentInfo &seg = segments.getSegment(indexList[0]);
    seg.setPatlakStartIndex(val);
    refreshPatlakData();
  }
  plot->replot();
}
void AnalyseDialog::on_sliderPatlakEnd_valueChanged(int val) {
  markerEnd->setXValue(times[val]);
  QModelIndexList indexList = listPatlak->selectionModel()->selectedRows();
  if (indexList.size() == 1) {
    SegmentInfo &seg = segments.getSegment(indexList.at(0));
    seg.setPatlakEndIndex(val);
    refreshPatlakData();
  }
  plot->replot();
}

void AnalyseDialog::on_tabWidget_currentChanged( int index ) {
  if (index == 2) onSliderClearanceChanged();
}


void AnalyseDialog::onSliderClearanceChanged() {
  markerStart->setXValue(times[sliderClearanceStart->value()]);
  markerEnd->setXValue(times[sliderClearanceEnd->value()]);
  plot->replot();
  onClearanceParametersChanged();
}

void AnalyseDialog::onClearanceParametersChanged(void ) {
  ClearanceCalculator calc(this);
  if (buttonLeftCortex->getSelectedSegment()!=NULL && buttonLeftArtery->getSelectedSegment()==NULL)
    buttonLeftArtery->setSelectedSegment( buttonLeftCortex->getSelectedSegment()->getArterySegment() );
  if (buttonRightCortex->getSelectedSegment()!=NULL && buttonRightArtery->getSelectedSegment()==NULL)
    buttonRightArtery->setSelectedSegment( buttonRightCortex->getSelectedSegment()->getArterySegment() );
  labelLeftClearance->setText(QString::number(calc.getLeftCTClearance()));
  labelRightClearance->setText(QString::number(calc.getRightCTClearance()));
  labelLeftClearance->repaint();
  labelRightClearance->repaint();
}

