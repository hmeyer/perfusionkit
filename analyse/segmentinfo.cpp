#include "segmentinfo.h"
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include "gammafitdata.h"
#include "gammaVariate.h"
#include "timedensitydata.h"
#include <binaryimagetreeitem.h>
#include <QObject>

SegmentInfo::SegmentInfo(const BinaryImageTreeItem *s):
  gammaStartIndex(0), gammaEndIndex(0), segment(s), arterySegment(NULL),
  sampleCurve(s->getName()), gammaCurve(s->getName()+ QObject::tr(" Gamma Fit")),
  patlakCurve(s->getName()), patlakRegression(s->getName() + QObject::tr(" linear Regression")) {
    
    const RGBType &RGBColor = s->getColor();
    QColor color = QColor( RGBColor[0], RGBColor[1], RGBColor[2] );
    sampleCurve.setPen(QPen(color));
    sampleCurve.setRenderHint(QwtPlotItem::RenderAntialiased, true);
    QwtSymbol symbol; 
    symbol.setStyle( QwtSymbol::Ellipse );
    symbol.setSize(8);
    symbol.setPen(QPen(color));
    symbol.setBrush(QBrush(color.darker(130)));
    sampleCurve.setSymbol( symbol );
    sampleCurve.setData( TimeDensityData() );      
    
    QPen pen(color);
    pen.setStyle(Qt::DotLine);
    gammaCurve.setPen(pen);
    gammaCurve.setRenderHint(QwtPlotItem::RenderAntialiased, true);
    gammaCurve.setData( GammaFitData() );
    gammaCurve.setVisible(false);
    
    patlakCurve.setVisible(false);
    patlakRegression.setVisible(false);
    
}

void SegmentInfo::pushSample(double time, const SegmentationValues &values) {
  dynamic_cast<TimeDensityData&>(sampleCurve.data()).pushPoint(time, values);
  dynamic_cast<GammaFitData&>(gammaCurve.data()).includeTime(time);
}

void SegmentInfo::attachSampleCurve(QwtPlot *plot) {
  sampleCurve.attach(plot);
}

void SegmentInfo::attachGammaCurve(QwtPlot *plot) {
  gammaCurve.attach(plot);
}


bool SegmentInfo::isGammaEnabled() const {
  return gammaCurve.isVisible();
}

void SegmentInfo::setEnableGamma(bool e) {
  gammaCurve.setVisible(e);
}

GammaFunctions::GammaVariate *SegmentInfo::getGamma() {
  return &(dynamic_cast<GammaFitData&>(gammaCurve.data()).getGammaVariate());
}

const GammaFunctions::GammaVariate *SegmentInfo::getGamma() const {
  return &(dynamic_cast<const GammaFitData&>(gammaCurve.data()).getGammaVariate());
}

double SegmentInfo::getGammaMaxSlope() const {
  return getGamma()->getMaxSlope();
}
double SegmentInfo::getGammaMaximum() const {
  return getGamma()->getMaximum();
}
double SegmentInfo::getGammaCenterOfGravity() const {
  return getGamma()->getCenterOfGravity();
}
double SegmentInfo::getGammaAUC() const {
  return getGamma()->getAUC();
}


void SegmentInfo::recalculateGamma() {
  GammaFunctions::GammaVariate * gamma = getGamma();
  gamma->clearSamples();
  const QwtData &tdd = sampleCurve.data();
  for(unsigned i = gammaStartIndex; i <= gammaEndIndex; ++i) {
    if (i < tdd.size()) gamma->addSample(tdd.x(i), tdd.y(i));
    else {
      std::cerr << "recalculateGamma:" << i << ">=" <<tdd.size() << std::endl;
    }
  }
  gamma->findFromSamples();
}

const QString &SegmentInfo::getName() const {
  if (segment!= NULL) return segment->getName();
  const static QString emptyString;
  return emptyString;
}

TimeDensityData *SegmentInfo::getSampleData() {
  return &(dynamic_cast<TimeDensityData&>(sampleCurve.data())); 
}
