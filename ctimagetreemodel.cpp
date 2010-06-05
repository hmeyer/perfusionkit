#include "ctimagetreemodel.h"
#include <QProgressDialog>
#include <boost/make_shared.hpp>

CTImageTreeModel::CTImageTreeModel(const DicomTagList &header, QObject *parent)
  : QAbstractItemModel(parent), TreeItem( NULL) {
    HeaderFields = boost::make_shared<DicomTagList>(header);
}

CTImageTreeModel::~CTImageTreeModel() {}

QVariant CTImageTreeModel::data(int column, int role) const {
  return QVariant::Invalid;
}

QVariant CTImageTreeModel::data(const QModelIndex &index, int role) const {
  return getItem(index).data( index.column(), role );
}

bool CTImageTreeModel::hasChildren ( const QModelIndex & parent) const {
  return (getItem(parent).childCount() > 0);
}

QVariant CTImageTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole) {
    if ( section < 0 || section >= int(HeaderFields->size()) ) return QVariant::Invalid;
    return QString::fromStdString((*HeaderFields)[section].first);
  } else {
    return QVariant::Invalid;
  }  
}

QModelIndex CTImageTreeModel::index(int row, int column, const QModelIndex &parent) const {
  const TreeItem &childItem = getItem(parent).child(row);
  try {
      return createIndex(row, column, &childItem);
  } catch( TreeItem::TreeTrouble &e ) {
    return QModelIndex();  
  }
}

QModelIndex CTImageTreeModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
      return QModelIndex();

  const TreeItem &childItem = getItem(index);
  const TreeItem *parentItem = childItem.parent();

  if (parentItem == this || parentItem == NULL)
      return QModelIndex();

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

int CTImageTreeModel::rowCount(const QModelIndex &parent) const {
  return getItem(parent).childCount();  
}

int CTImageTreeModel::columnCount(const QModelIndex &parent) const {
  return getItem(parent).columnCount();
}

Qt::ItemFlags CTImageTreeModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;
  return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;  
}

bool CTImageTreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (role == Qt::EditRole) return getItem(index).setData(index.column(), value);
  else return false;
}

TreeItem &CTImageTreeModel::getItem(const QModelIndex &index) {
  if (index.isValid()) {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item) return *item;
  }
  return *this;
}

const TreeItem &CTImageTreeModel::getItem(const QModelIndex &index) const {
  if (index.isValid()) {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item) return *item;
  }
  return *this;
}

void CTImageTreeModel::appendFilename( const itk::MetaDataDictionary &dict, const std::string &fname) {
  std::string iUID;
  CTImageTreeItem::getUIDFromDict( dict, iUID );
  if (iUID.empty()) return;
  bool found = false;
  CTImageTreeItem *c;
  int cc = childCount();
  for(int i=0; i < cc; i++) {
    c = dynamic_cast<CTImageTreeItem*>(&child(i));
    if (c->getUID() == iUID) {
      found = true;
      break;
    }
  }
  if (!found) {
    c = new CTImageTreeItem( this, HeaderFields, dict );
    beginInsertRows(QModelIndex(),cc,cc);
    insertChild(c);
    endInsertRows();
  }
  c->appendFileName(fname);
}

void CTImageTreeModel::insertItemCopy(const TreeItem& item) {
  int cc = childCount();
  beginInsertRows(QModelIndex(),cc,cc);
  insertChild(item.clone(this));
  endInsertRows();
}

bool CTImageTreeModel::removeCTImage(int srow) {
  unsigned int row = static_cast<unsigned int>(srow);
  if (row < childCount()) {
    beginRemoveRows(QModelIndex(), row, row);
    bool result = removeChildren( row, 1 );
    endRemoveRows();
    return result;
  }
  return false;
}

bool CTImageTreeModel::createSegment(int srow) {
  unsigned int row = static_cast<unsigned int>(srow);
  if (row < childCount()) {
    beginInsertRows(createIndex(row, 0, this), row, row);
    bool result = dynamic_cast<CTImageTreeItem&>(child(row)).generateSegment();
    endInsertRows();
    return result;
  }
  return false;
}


void CTImageTreeModel::loadAllImages(void) {
  const int progressScale = 10000;
  QProgressDialog progressDialog(tr("Loading Volumes..."), tr("Abort"), 0, progressScale);
  progressDialog.setMinimumDuration(1000);
  progressDialog.setWindowModality(Qt::WindowModal);
  const int scalePerVolume = progressScale/childCount();
  for(unsigned int i=0; i < childCount(); i++) {
    dynamic_cast<CTImageTreeItem&>(child(i)).getVTKImage(&progressDialog, scalePerVolume, scalePerVolume*i );
    if (progressDialog.wasCanceled()) break;
  }
}

 
QModelIndex CTImageTreeModel::createIndex(int r, int c, const TreeItem*p) const {
  return QAbstractItemModel::createIndex(r, c, const_cast<TreeItem*>(p));
}
