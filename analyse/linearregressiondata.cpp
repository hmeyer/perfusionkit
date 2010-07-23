#include "linearregressiondata.h"
#include "linearregression.h"

double LinearRegressionData::y(size_t i) const {
  const_cast<LinearRegressionData*>(this)->update();
  return m * x(i) + n;
}

double LinearRegressionData::getIntercept() const { 
  const_cast<LinearRegressionData*>(this)->update();
  return n; 
}
double LinearRegressionData::getSlope() const { 
  const_cast<LinearRegressionData*>(this)->update();
  return m; 
}

double LinearRegressionData::x(size_t i) const {
  if (i==0) return source.x(0);
  else return source.x(source.size()-1);
}
size_t LinearRegressionData::size() const {
  if (source.size()>1) return 2;
  else return 0;
}

void LinearRegressionData::update(void) {
  bool recalc = false;
  if (xc.size() != source.size()) {
    recalc = true;
    xc.resize( source.size() );
    yc.resize( source.size() );
  }
  double t;
  for(unsigned i=0; i < source.size(); ++i) {
    t = source.x(i);
    if (xc[i] != t) { xc[i] = t; recalc = true; }
    t = source.y(i);
    if (yc[i] != t) { yc[i] = t; recalc = true; }
  }
  if (recalc) LinearRegression(xc.begin(),xc.end(),yc.begin(),n,m);
}