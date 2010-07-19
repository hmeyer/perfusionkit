#ifndef GAMMAFITDATA_H
#define GAMMAFITDATA_H

#include <qwt_data.h>
#include <boost/shared_ptr.hpp>

namespace GammaFunctions {
class GammaVariate;
}

const unsigned GammaSamples = 500;
class GammaFitData: public QwtData {
public:
  GammaFitData(boost::shared_ptr<GammaFunctions::GammaVariate> g, double start, double end):
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
    virtual double y(size_t i) const;
    boost::shared_ptr<GammaFunctions::GammaVariate> getGammaVariate() { return gamma; }
private:
  double xstart, xend;
  boost::shared_ptr<GammaFunctions::GammaVariate> gamma;
};


#endif // GAMMAFITDATA_H
