#ifndef TIMEDENSITYDATA_H
#define TIMEDENSITYDATA_H

#include <qwt_data.h>
#include "segmentationvalues.h"

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
    double getTimeAndValues( size_t i, SegmentationValues &v ) const {
      v = values[i];
      return time[i];
    }
    void pushPoint( double t, const SegmentationValues &v) {
      if (time.size() == 0 || t> time[time.size()-1]) {
	time.push_back(t);
	values.push_back(v);
      }
    }
private:
  std::vector< double > time;
  std::vector< SegmentationValues > values;
};


#endif // TIMEDENSITYDATA_H
