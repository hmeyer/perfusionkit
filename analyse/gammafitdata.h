#ifndef GAMMAFITDATA_H
#define GAMMAFITDATA_H

#include <qwt_data.h>
#include <limits>
#include "gammaVariate.h"

const unsigned GammaSamples = 500;
class GammaFitData: public QwtData {
public:
  GammaFitData():
    xstart(std::numeric_limits<double>::max()), xend(std::numeric_limits<double>::min()) { }
    virtual QwtData *copy() const {
        return new GammaFitData(*this);
    }
    virtual size_t size() const {
        return GammaSamples;
    }
    virtual double x(size_t i) const {
        return xstart + (xend - xstart) / GammaSamples * i;
    }
    virtual double y(size_t i) const {
      return gamma.computeY(x(i));
    }
    GammaFunctions::GammaVariate &getGammaVariate() { return gamma; }
    const GammaFunctions::GammaVariate &getGammaVariate() const { return gamma; }
    void includeTime(double t) {
      if (t>xend) xend = t;
      if (t<xstart) xstart = t;
    }
private:
  double xstart, xend;
  GammaFunctions::GammaVariate gamma;
};


#endif // GAMMAFITDATA_H
