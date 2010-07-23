#ifndef LINEARREGRESSION_H
#define LINEARREGRESSION_H

#include <limits>

template<class ForwardIt, class float_t>
void LinearRegression(ForwardIt beginX, ForwardIt endX, ForwardIt beginY, float_t &b0, float_t &b1) {
  unsigned int n = 0;
  float_t sx = 0.0, sy = 0.0, sx2 = 0.0, sxy = 0.0;
  while(beginX < endX) {
    sx += *beginX;
    sx2 += *beginX * *beginX;
    sy += *beginY;
    sxy += *beginX * *beginY;
    n++;
    beginX++;
    beginY++;
  }
  float_t a = n;
  float_t b = sx;
  float_t c = sx;
  float_t d = sx2;
  float_t e = sy;
  float_t f = sxy;
  float_t detA = (a*d - b*c);
  if (detA != 0) {
    b0 = (e*d - b*f) / detA;
    b1 = (a*f - e*c) / detA;
  } else {
    b0 = std::numeric_limits< float_t >::max();
    b1 = std::numeric_limits< float_t >::max();
  }
}

#endif // LINEARREGRESSION_H