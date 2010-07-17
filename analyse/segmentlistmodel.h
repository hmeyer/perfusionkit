#ifndef SEGMENTLISTMODEL_H
#define SEGMENTLISTMODEL_H

#include <qt4/QtCore/QAbstractItemModel>
#include <vector>
#include "binaryimagetreeitem.h"


class SegmentListModel : public QAbstractListModel
{
  public:
    struct SegmentInfo {
      const BinaryImageTreeItem *segment;
      bool enableGammaFit;
      unsigned gammaStartIndex;
      unsigned gammaEndIndex;
      double gammaStart;
      double gammaEnd;
      SegmentInfo(const BinaryImageTreeItem *s):segment(s), enableGammaFit(false), 
	gammaStartIndex(0), gammaEndIndex(0), gammaStart(0), gammaEnd(0) {}
    };    
    typedef std::vector<SegmentInfo> SegmentListType;
    typedef SegmentListType::iterator iterator;
    SegmentListModel(QObject *parent = 0);
    void addSegment( const BinaryImageTreeItem *seg );
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    SegmentInfo &getSegment(const QModelIndex& index);
    iterator begin() { return segments.begin(); }
    iterator end() { return segments.end(); }
  private:
    SegmentListType segments;
};

#endif // SEGMENTLISTMODEL_H
