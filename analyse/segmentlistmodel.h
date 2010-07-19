#ifndef SEGMENTLISTMODEL_H
#define SEGMENTLISTMODEL_H

#include <qt4/QtCore/QAbstractItemModel>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "binaryimagetreeitem.h"

namespace GammaFunctions {
class GammaVariate;
}

class SegmentListModel : public QAbstractListModel
{
  public:
    struct SegmentInfo {
      const BinaryImageTreeItem *segment;
      unsigned gammaStartIndex;
      unsigned gammaEndIndex;
      double gammaStart;
      double gammaEnd;
      const SegmentInfo *arterySegment;
      boost::shared_ptr<GammaFunctions::GammaVariate> gamma;
      SegmentInfo(const BinaryImageTreeItem *s):segment(s), 
	gammaStartIndex(0), gammaEndIndex(0), gammaStart(0), gammaEnd(0), arterySegment(NULL)  {}
    };    
    typedef std::vector<SegmentInfo> SegmentListType;
    typedef SegmentListType::iterator iterator;
    typedef SegmentListType::const_iterator const_iterator;
    SegmentListModel(QObject *parent = 0);
    void addSegment( const BinaryImageTreeItem *seg );
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    void refresh();
    
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    SegmentInfo &getSegment(const QModelIndex& index);
    SegmentInfo &getSegment(unsigned row);
    void setArterySegment(const QModelIndex& index, const SegmentInfo *arterySegment);
    const_iterator begin() const { return segments.begin(); }
    const_iterator end() const { return segments.end(); }
    iterator begin() { return segments.begin(); }
    iterator end() { return segments.end(); }
  private:
    SegmentListType segments;
};

#endif // SEGMENTLISTMODEL_H
