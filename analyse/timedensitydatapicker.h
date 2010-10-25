/*
    This file is part of perfusionkit.
    Copyright 2010 Henning Meyer

    perfusionkit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    perfusionkit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with perfusionkit.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMEDENSITYDATAPICKER_H
#define TIMEDENSITYDATAPICKER_H

#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include "analysedialog.h"

class TimeDensityDataPicker: public QwtPlotPicker {
  public:
  TimeDensityDataPicker(QwtPlotMarker *markerX_, QwtPlotMarker *markerY_, const SegmentListModel &segmentList_, QwtPlotCanvas *);
  virtual QwtText trackerText(const QPoint &) const;
  protected:
    QwtPlotMarker *markerX;
    QwtPlotMarker *markerY;
    const SegmentListModel &segmentList;
};

#endif // TIMEDENSITYDATAPICKER_H
