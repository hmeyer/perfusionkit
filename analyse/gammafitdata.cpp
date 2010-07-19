#include "gammafitdata.h"
#include "gammaVariate.h"

double GammaFitData::y(size_t i) const {
  if (!gamma) return 0;
  return gamma->computeY(x(i));
}
