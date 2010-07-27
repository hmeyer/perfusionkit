#include "segmentinfo.h"
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include "gammafitdata.h"
#include "gammaVariate.h"
#include "timedensitydata.h"
#include "patlakdata.h"
#include "linearregressiondata.h"
#include "binaryimagetreeitem.h"
#include <QObject>

SegmentInfo::SegmentInfo(const BinaryImageTreeItem *s):
  gammaStartIndex(0), gammaEndIndex(0), segment(s), arterySegment(NULL),
  sampleCurve(s->getName()), gammaCurve(s->getName()+ QObject::tr(" Gamma Fit")),
  patlakCurve(s->getName()), patlakRegression(s->getName() + QObject::tr(" linear Regression")),
  patlakCreated(false) {
    
    QColor color = s->getColor();
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

    patlakCurve.setRenderHint(QwtPlotItem::RenderAntialiased, true);
    patlakCurve.setStyle(QwtPlotCurve::NoCurve);
    symbol.setStyle( QwtSymbol::XCross );
    symbol.setSize(8);
    symbol.setPen(QPen(color));
    symbol.setBrush(QBrush(color.darker(130)));
    patlakCurve.setSymbol( symbol );
    patlakRegression.setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

bool SegmentInfo::createPatlak() {
  if (patlakCreated) return true;
  if (!isGammaEnabled() || arterySegment==NULL || !arterySegment->isGammaEnabled())
    return false;
  PatlakData pd(sampleCurve.data(), arterySegment->sampleCurve.data());
  pd.setTissueBaseline(getGamma()->getBaseline());
  pd.setArteryBaseline(arterySegment->getGamma()->getBaseline());
  patlakCurve.setData( pd );
  LinearRegressionData patlakRegressionData( patlakCurve.data() );
  patlakRegression.setData(patlakRegressionData);
  patlakCreated = true;
  return true;
}

unsigned SegmentInfo::getPatlakStartIndex() const {
  if (patlakCreated) {
    return dynamic_cast<const PatlakData&>(patlakCurve.data()).getStartIndex();
  } else return 0;
}
unsigned SegmentInfo::getPatlakEndIndex() const {
  if (patlakCreated) {
    return dynamic_cast<const PatlakData&>(patlakCurve.data()).getEndIndex();
  } else return 0;
}
void SegmentInfo::setPatlakStartIndex(unsigned index) {
  if (patlakCreated) {
    dynamic_cast<PatlakData&>(patlakCurve.data()).setStartIndex(index);
  }
}
void SegmentInfo::setPatlakEndIndex(unsigned index) {
  if (patlakCreated) {
    dynamic_cast<PatlakData&>(patlakCurve.data()).setEndIndex(index);
  }
}
double SegmentInfo::getPatlakIntercept() const {
  if (patlakCreated) {
    return dynamic_cast<const LinearRegressionData&>(patlakRegression.data()).getIntercept();
  } else return std::numeric_limits< double >::quiet_NaN();
}
double SegmentInfo::getPatlakSlope() const {
  if (patlakCreated) {
    return dynamic_cast<const LinearRegressionData&>(patlakRegression.data()).getSlope();
  } else return std::numeric_limits< double >::quiet_NaN();
}




void SegmentInfo::pushSample(double time, const SegmentationValues &values) {
  dynamic_cast<TimeDensityData&>(sampleCurve.data()).pushPoint(time, values);
  dynamic_cast<GammaFitData&>(gammaCurve.data()).includeTime(time);
}

void SegmentInfo::attachSampleCurves(QwtPlot *plot) {
  sampleCurve.attach(plot);
  gammaCurve.attach(plot);
}

bool SegmentInfo::attachPatlak(QwtPlot *plot) {
  if (!createPatlak()) return false;
  patlakCurve.attach(plot);
  patlakRegression.attach(plot);
  return true;
}

void SegmentInfo::detachPatlak() {
  patlakCurve.detach();
  patlakRegression.detach();
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
double SegmentInfo::getGammaBaseline() const {
  return getGamma()->getBaseline();
}


void SegmentInfo::recalculateGamma() {
  GammaFunctions::GammaVariate * gamma = getGamma();
  gamma->clearSamples();
  const QwtData &tdd = sampleCurve.data();
  for(unsigned i = gammaStartIndex; i <= gammaEndIndex; ++i) {
    if (i < tdd.size()) gamma->addSample(tdd.x(i), tdd.y(i));
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

const TimeDensityData *SegmentInfo::getSampleData() const {
  return &(dynamic_cast<const TimeDensityData&>(sampleCurve.data())); 
}
