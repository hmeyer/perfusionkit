#ifndef SEGMENTLISTMODEL_H
#define SEGMENTLISTMODEL_H

#include <qt4/QtCore/QAbstractItemModel>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "segmentinfo.h"


class SegmentListModel : public QAbstractListModel
{
  public:
    typedef boost::shared_ptr<SegmentInfo> SegmentInfoPtr;
    typedef std::vector<SegmentInfoPtr> SegmentListType;
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
