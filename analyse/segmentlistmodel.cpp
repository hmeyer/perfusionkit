#include "segmentlistmodel.h"


SegmentListModel::SegmentListModel(QObject *parent):
  QAbstractListModel(parent) {
}


QVariant SegmentListModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole && index.column() == 0 && index.row() < int(segments.size())) {
    return segments[index.row()].segment->getName();
  }
  return QVariant::Invalid;
}

SegmentListModel::SegmentInfo &SegmentListModel::getSegment(const QModelIndex& index) {
  return segments.at( index.row() );
}


int SegmentListModel::rowCount(const QModelIndex& parent) const
{
  return segments.size();
}



void SegmentListModel::addSegment( const BinaryImageTreeItem *seg ) {
  beginInsertRows(QModelIndex(), segments.size(), segments.size()+1);
  segments.push_back(const_cast<BinaryImageTreeItem*>(seg));
  endInsertRows();
}
