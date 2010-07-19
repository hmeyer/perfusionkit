#ifndef TIMEDENSITYDATAPICKER_H
#define TIMEDENSITYDATAPICKER_H

#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include "analysedialog.h"

class TimeDensityDataPicker: public QwtPlotPicker {
  public:
  TimeDensityDataPicker(QwtPlotMarker *markerX_, QwtPlotMarker *markerY_, const AnalyseDialog::SegmentCurveMap &curveset_, QwtPlotCanvas *);
  virtual QwtText trackerText(const QPoint &) const;
  protected:
    QwtPlotMarker *markerX;
    QwtPlotMarker *markerY;
    const AnalyseDialog::SegmentCurveMap &curveset;
};

#endif // TIMEDENSITYDATAPICKER_H
