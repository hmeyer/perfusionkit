#include "patlakdata.h"
#include <algorithm>

PatlakData::PatlakData(const QwtData &tissueSource, const QwtData &arterySource)
  :tissue(tissueSource), artery(arterySource), 
  tissueBaseline(0), arteryBaseline(0),
  startIndex(0), endIndex(0) {}

double PatlakData::y(size_t i) const {
  return tissue.y(i+startIndex) / artery.y(i+startIndex);
}

double PatlakData::x(size_t i) const {
  double lastTissueTime = tissue.x(startIndex);
  double lastTissueDensity = tissue.y(startIndex);
  double integral = .0;
  for(unsigned k = startIndex+1; k < (startIndex+i+1); ++k) {
    double tissueTime = tissue.x(k);
    double tissueDensity = tissue.y(k);
    integral += (lastTissueDensity + tissueDensity) * .5 * (tissueTime - lastTissueTime);
    lastTissueTime = tissueTime;
    lastTissueDensity = tissueDensity;
  }
  return integral / tissue.y(i);
}

size_t PatlakData::size() const  { 
  signed d = endIndex+1;
  d-=startIndex;
  return std::max(d,1); 
}
