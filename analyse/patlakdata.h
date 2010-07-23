#ifndef PATLAKDATA_H
#define PATLAKDATA_H

#include <qwt_data.h>


class PatlakData : public QwtData
{
  public:
    PatlakData( const QwtData &tissueSource, const QwtData &arterySource );
    virtual double y(size_t i) const;
    virtual double x(size_t i) const;
    virtual size_t size() const;
    virtual QwtData* copy() const { return new PatlakData( *this ); }
    inline void setTissueBaseline(double tb) { tissueBaseline = tb; }
    inline void setArteryBaseline(double ab) { arteryBaseline = ab; }
    inline size_t getStartIndex() const { return startIndex; }
    inline size_t getEndIndex() const { return endIndex; }
    inline void setStartIndex(size_t index) { startIndex = std::min(index,std::min(tissue.size(), artery.size())-2); }
    inline void setEndIndex(size_t index) { endIndex = std::min(index,std::min(tissue.size(), artery.size())-2);  }
  private:
    const QwtData &tissue;
    const QwtData &artery;
    double tissueBaseline, arteryBaseline;
    size_t startIndex, endIndex;
};

#endif // PATLAKDATA_H
