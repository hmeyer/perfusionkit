#include "segmentlistmodel.h"
#include "gammaVariate.h"


SegmentListModel::SegmentListModel(QObject *parent):
  QAbstractListModel(parent) {
}


QVariant SegmentListModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole && index.row() < int(segments.size())) {
    const SegmentInfo &seg = segments[index.row()];
    switch(index.column()) {
      case 0: return seg.segment->getName();
      case 1: if (seg.gamma) return seg.gamma->getMaxSlope();break;
      case 2: if (seg.gamma) return seg.gamma->getMaximum();break;
      case 3: if (seg.arterySegment != NULL) return seg.arterySegment->segment->getName(); break;
      case 4: if (seg.gamma && seg.arterySegment!=NULL && seg.arterySegment->gamma) return seg.gamma->getMaxSlope() / seg.arterySegment->gamma->getMaximum();break;
    }
  }
  return QVariant::Invalid;
}

QVariant SegmentListModel::headerData( int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch( section ) {
      case 0: return tr("segment");
      case 1: return tr("max slope [HU/s]");
      case 2: return tr("enhancement [HU]");
      case 3: return tr("artery");
      case 4: return tr("perfusion [/s]");
    }
  }
  return QVariant::Invalid;
}

int SegmentListModel::columnCount(const QModelIndex & parent) const {
  return 5;
}


SegmentListModel::SegmentInfo &SegmentListModel::getSegment(const QModelIndex& index) {
  return segments.at( index.row() );
}


int SegmentListModel::rowCount(const QModelIndex& parent) const
{
  return segments.size();
}

void SegmentListModel::setArterySegment(const QModelIndex& index, const SegmentInfo *arterySegment) {
  if (index.row() < int(segments.size()) ) {
    segments[index.row()].arterySegment = arterySegment;
    emit dataChanged(this->index(index.row(),1), this->index(index.row(),4));
  }
}

void SegmentListModel::refresh() {
    emit dataChanged(this->index(0,1), this->index(segments.size()-1,4));
}


void SegmentListModel::addSegment( const BinaryImageTreeItem *seg ) {
  beginInsertRows(QModelIndex(), segments.size(), segments.size()+1);
  segments.push_back(const_cast<BinaryImageTreeItem*>(seg));
  endInsertRows();
}
