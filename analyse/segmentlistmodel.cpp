#include "segmentlistmodel.h"
#include <qwt_plot_curve.h>
#include "gammafitdata.h"
#include "gammaVariate.h"
#include "binaryimagetreeitem.h"

SegmentListModel::SegmentListModel(QObject *parent):
  QAbstractListModel(parent) {
}


QVariant SegmentListModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole && index.row() < int(segments.size())) {
    const SegmentInfo &seg = segments[index.row()];
    switch(index.column()) {
      case 0: return seg.getName();
      case 1: if (seg.isGammaEnabled()) return seg.getGammaMaxSlope();break;
      case 2: if (seg.isGammaEnabled()) return seg.getGammaMaximum();break;
      case 3: if (seg.getArterySegment() != NULL) return seg.getArterySegment()->getName(); break;
      case 4: if (seg.isGammaEnabled()) {
	if (seg.getArterySegment()!=NULL && seg.getArterySegment()->isGammaEnabled())
	  return 60 * seg.getGammaMaxSlope() / seg.getArterySegment()->getGammaMaximum();
      } break;
      case 5: if (seg.isGammaEnabled()) return seg.getGammaCenterOfGravity();break;
      case 6: if (seg.isGammaEnabled()) return seg.getGammaAUC();break;
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
      case 4: return tr("perfusion [/min]");
      case 5: return tr("MTT [s]");
      case 6: return tr("AUC");
    }
  }
  return QVariant::Invalid;
}

int SegmentListModel::columnCount(const QModelIndex & parent) const {
  return 7;
}


SegmentInfo &SegmentListModel::getSegment(const QModelIndex& index) {
  return segments.at( index.row() );
}


int SegmentListModel::rowCount(const QModelIndex& parent) const
{
  return segments.size();
}

void SegmentListModel::setArterySegment(const QModelIndex& index, const SegmentInfo *arterySegment) {
  if (index.row() < int(segments.size()) ) {
    segments[index.row()].setArterySegment( arterySegment );
    emit dataChanged(this->index(index.row(),1), this->index(index.row(),4));
  }
}

void SegmentListModel::refresh() {
    emit dataChanged(this->index(0,1), this->index(segments.size()-1,4));
}


void SegmentListModel::addSegment( const BinaryImageTreeItem *seg ) {
  beginInsertRows(QModelIndex(), segments.size(), segments.size()+1);
  segments.push_back(new SegmentInfo(const_cast<BinaryImageTreeItem*>(seg)));
  endInsertRows();
}
