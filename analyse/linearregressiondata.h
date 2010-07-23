#ifndef LINEARREGRESSIONDATA_H
#define LINEARREGRESSIONDATA_H
#include <vector>

#include <qwt_data.h>

class LinearRegressionData : public QwtData
{
  public:
    LinearRegressionData( const QwtData &source_ ):source(source_), m(0), n(0) { update();}
    virtual double y(size_t i) const;
    virtual double x(size_t i) const;
    virtual size_t size() const;
    virtual QwtData* copy() const { return new LinearRegressionData( source ); }
    void update(void);
    double getIntercept() const;
    double getSlope() const;
  private:
    const QwtData &source;
    double m, n;
    std::vector< double > xc,yc;
};
#endif // LINEARREGRESSIONDATA_H
