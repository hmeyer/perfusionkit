#ifndef SEGMENTINFO_H
#define SEGMENTINFO_H

#include <boost/shared_ptr.hpp>
#include <qwt_plot_curve.h>
#include <QString>

class BinaryImageTreeItem;
class QwtPlotCurve;
class TimeDensityData;
class GammaFitData;
class QwtPlot;
namespace GammaFunctions {
class GammaVariate;
}

class SegmentationValues;


class SegmentInfo {
  public:
  SegmentInfo(const BinaryImageTreeItem *s);
  const QString &getName() const;
  inline const BinaryImageTreeItem *getSegment() const {return segment;}
  inline void setSegment(BinaryImageTreeItem *seg) {segment = seg;}
  inline const SegmentInfo *getArterySegment() const {return arterySegment;}
  inline void setArterySegment(const SegmentInfo *seg) {arterySegment = seg;}
  void attachSampleCurves(QwtPlot *plot);
  void pushSample(double time, const SegmentationValues &value);

  bool isGammaEnabled() const;
  void setEnableGamma(bool e=true);
  GammaFunctions::GammaVariate *getGamma();
  const GammaFunctions::GammaVariate *getGamma() const;
  double getGammaMaxSlope() const;
  double getGammaMaximum() const;
  double getGammaCenterOfGravity() const;
  double getGammaAUC() const;
  double getGammaBaseline() const;
  inline void setGammaStartIndex(unsigned index) { gammaStartIndex = index; }
  inline void setGammaEndIndex(unsigned index)  { gammaEndIndex = index; }
  inline unsigned getGammaStartIndex() const { return gammaStartIndex; }
  inline unsigned getGammaEndIndex() const { return gammaEndIndex; }
  
  bool attachPatlak(QwtPlot *plot);
  void detachPatlak();
  unsigned getPatlakStartIndex() const;
  unsigned getPatlakEndIndex() const;
  double getPatlakIntercept() const;
  double getPatlakSlope() const;
  void setPatlakStartIndex(unsigned index);
  void setPatlakEndIndex(unsigned index);
  TimeDensityData *getSampleData();
  const TimeDensityData *getSampleData() const;
  void recalculateGamma();
  private:
  bool createPatlak();
  unsigned gammaStartIndex;
  unsigned gammaEndIndex;
  const BinaryImageTreeItem *segment;
  const SegmentInfo *arterySegment;
  QwtPlotCurve sampleCurve;
  QwtPlotCurve gammaCurve;
  QwtPlotCurve patlakCurve;
  QwtPlotCurve patlakRegression;
  bool patlakCreated;
};

#endif // SEGMENTINFO_H
