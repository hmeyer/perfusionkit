#include "ctimagetreemodel.h"
#include <QProgressDialog>
#include <QApplication>
#include <boost/make_shared.hpp>

CTImageTreeModel::CTImageTreeModel(const DicomTagList &header, QObject *parent)
  : QAbstractItemModel(parent), rootItem( this ) {
    HeaderFields = boost::make_shared<DicomTagList>(header);
}

CTImageTreeModel::~CTImageTreeModel() {}

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

void CTImageTreeModel::sort(int column, Qt::SortOrder order) {
  rootItem.sortChildren(column, order == Qt::AscendingOrder);
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

  if (parentItem == &rootItem || parentItem == NULL)
      return QModelIndex();

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

int CTImageTreeModel::rowCount(const QModelIndex &parent) const {
  return getItem(parent).childCount();  
}

int CTImageTreeModel::columnCount(const QModelIndex &parent) const {
//  const TreeItem *parentItem = &getItem(parent);
//  if (parentItem != &rootItem) return parentItem->columnCount();
  return HeaderFields->size();
}

Qt::ItemFlags CTImageTreeModel::flags(const QModelIndex &index) const {
  return getItem(index).flags(index.column());
}

QVariant CTImageTreeModel::data(const QModelIndex &index, int role) const {
  return getItem(index).data(index.column(), role);
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
  return rootItem;
}

const TreeItem &CTImageTreeModel::getItem(const QModelIndex &index) const {
  if (index.isValid()) {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item) return *item;
  }
  return rootItem;
}
/*
bool CTImageTreeModel::isActive(const TreeItem *item) const {
  ItemPointerContainer::const_iterator it = activeItems.find( item );
  if (it != activeItems.end()) return true;
  return false;
}

void CTImageTreeModel::setActive(const TreeItem *item, bool active) {
  if (active) activeItems.insert(item);
  else activeItems.erase(item);
  dataChanged(createIndex(0,0,item),createIndex(0,item->columnCount()-1,item));
}

void CTImageTreeModel::clearAllActive(void) {
  ItemPointerContainer t;
  std::swap(t, activeItems);
  for(ItemPointerContainer::const_iterator it = t.begin(); it!=t.end(); ++it) {
    dataChanged(createIndex(0, 0, *it), createIndex(0, (*it)->columnCount()-1, *it) );
  }
}
*/

void CTImageTreeModel::appendFilename( const itk::MetaDataDictionary &dict, const std::string &fname) {
  std::string iUID;
  CTImageTreeItem::getUIDFromDict( dict, iUID );
  if (iUID.empty()) return;
  bool found = false;
  CTImageTreeItem *c;
  int cc = rootItem.childCount();
  for(int i=0; i < cc; i++) {
    c = dynamic_cast<CTImageTreeItem*>(&rootItem.child(i));
    if (c->getUID() == iUID) {
      found = true;
      break;
    }
  }
  if (!found) {
    c = new CTImageTreeItem( &rootItem, HeaderFields, dict );
    rootItem.insertChild(c);
  }
  c->appendFileName(fname);
}

void CTImageTreeModel::insertItemCopy(const TreeItem& item) {
  rootItem.insertChild(item.clone(&rootItem));
}

bool CTImageTreeModel::removeCTImage(int srow) {
  unsigned int row = static_cast<unsigned int>(srow);
  if (row < rootItem.childCount()) {
    bool result = rootItem.removeChildren( row );
    return result;
  }
  return false;
}

bool CTImageTreeModel::createSegment(int srow) {
  unsigned int row = static_cast<unsigned int>(srow);
  if (row < rootItem.childCount()) {
    return dynamic_cast<CTImageTreeItem&>(rootItem.child(row)).generateSegment();
  }
  return false;
}

void CTImageTreeModel::loadAllImages(void) {
  const int progressScale = 10000;
  QProgressDialog progressDialog(tr("Loading Volumes..."), tr("Abort"), 0, progressScale);
  progressDialog.setMinimumDuration(1000);
  progressDialog.setWindowModality(Qt::WindowModal);
  const int scalePerVolume = progressScale/rootItem.childCount();
  for(unsigned int i=0; i < rootItem.childCount(); i++) {
    dynamic_cast<CTImageTreeItem&>(rootItem.child(i)).getVTKImage(&progressDialog, scalePerVolume, scalePerVolume*i );
    if (progressDialog.wasCanceled()) break;
  }
}

 
QModelIndex CTImageTreeModel::createIndex(int r, int c, const TreeItem*p) const {
  if (p == &rootItem) return QModelIndex();
  return QAbstractItemModel::createIndex(r, c, const_cast<TreeItem*>(p));
}
