#ifndef GAMMAFITDATA_H
#define GAMMAFITDATA_H

#include <qwt_data.h>
#include "gammaVariate.h"

const unsigned GammaSamples = 500;
class GammaFitData: public QwtData {
public:
  GammaFitData(const GammaFunctions::GammaVariate &g, double start, double end):
    xstart(start), xend( end ), gamma(g) { }
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
private:
  double xstart, xend;
  GammaFunctions::GammaVariate gamma;
};


#endif // GAMMAFITDATA_H
