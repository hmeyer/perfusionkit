#include "treeitem.h"
#include "boost/bind.hpp"
#include <iostream>
#include <QApplication>
#include <QFont>

#include "ctimagetreemodel.h"

TreeItem::TreeItem(CTImageTreeModel *model):model(model), parentItem(NULL),active(false) {
}

TreeItem::TreeItem(TreeItem * parent):model(parent->model), parentItem(parent),active(false) {
}

TreeItem::~TreeItem() {
}

void TreeItem::cloneChildren(TreeItem *dest) const {
  for(ChildListType::const_iterator it=childItems.begin(); it!=childItems.end(); it++)
    dest->insertChild( it->clone(dest) );
}


TreeItem &TreeItem::child(unsigned int number) {
  if (number >= childItems.size()) throw TreeTrouble();
  return childItems[number];
}

const TreeItem &TreeItem::child(unsigned int number) const {
  if (number >= childItems.size()) throw TreeTrouble();
  return childItems[number];
}

TreeItem *TreeItem::clone(TreeItem *clonesParent) const {
  TreeItem *c = new TreeItem( clonesParent );
  cloneChildren(c);
  return c;
}

int TreeItem::columnCount() const {
  return 0;
}

QVariant TreeItem::data(int column, int role) const {
  switch(role) {
    case Qt::DisplayRole: return do_getData_DisplayRole( column );
    case Qt::FontRole: return do_getData_FontRole( column );
    case Qt::ForegroundRole: return do_getData_ForegroundRole( column );
    case Qt::BackgroundRole: return do_getData_BackgroundRole( column );
  }
  return QVariant::Invalid;
}

QVariant TreeItem::do_getData_FontRole(int column) const {
  if (active) {
    QFont f = QApplication::font();
    f.setWeight( QFont::Bold );
    return f;
  }
  return QVariant::Invalid;
}


unsigned int TreeItem::childCount() const {
  return childItems.size();
}

class TreeItemCompareFunctor {
  int col;
  bool descending;
  public:
    typedef TreeItem first_argument_type;
    typedef TreeItem second_argument_type;
    typedef bool result_type;
  TreeItemCompareFunctor(int column, bool ascending=true):col(column),descending(!ascending) {};
  bool operator()(const TreeItem &x, const TreeItem &y) const {
    const QVariant qx = x.data(col);
    const QVariant qy = y.data(col);
    bool okx, oky;
    double dx = qx.toDouble(&okx);
    double dy = qy.toDouble(&oky);
    if (okx && oky) {
      if (dx == dy) return false;
      return descending ^ (dx < dy);
    } else if (okx) {
      return descending;
    } else if (oky) {
      return !descending;
    } else {
      if (&x == &y) return false;
      return descending ^ (&x < &y);
    }
  }
};

void TreeItem::sortChildren( int column, bool ascending ) {
  model->emitLayoutAboutToBeChanged();
  childItems.sort(
    TreeItemCompareFunctor( column, ascending ) );
  model->emitLayoutChanged();
}

unsigned int TreeItem::depth(void) const {
  const TreeItem *parent = parentItem;
  unsigned int depth = 0;
  while( parent != NULL ) {
    parent = parent->parentItem;
    depth++;
  }
  return depth;
}

bool TreeItem::insertChild(TreeItem *child) {
  return insertChild(child, childItems.size());
}

class TreeItemEqualFunctor {
    const std::string &uidx;
  public:
    typedef TreeItem first_argument_type;
    typedef TreeItem second_argument_type;
    typedef bool result_type;
  TreeItemEqualFunctor( const TreeItem &item ):uidx( item.getUID() ) {}
  bool operator()(const TreeItem &y) const {
    if (uidx.empty()) return false;
    const std::string uidy = y.getUID();
    if (uidy.empty()) return false;
    return uidx == uidy;
  }
};

bool TreeItem::insertChild(TreeItem *child, unsigned int position) {
  if (position > childItems.size())
    return false;
  if (std::find_if(childItems.begin(), childItems.end(), TreeItemEqualFunctor(*child)) != childItems.end())
    return false;

  QModelIndex insertIndex = model->createIndex(position,0,this);
  model->emitLayoutAboutToBeChanged();
  model->beginInsertRows(insertIndex,position,position);
  child->parentItem = this;
  child->model = this->model;
  childItems.insert(childItems.begin() + position, child);
  model->endInsertRows();
  model->emitLayoutChanged();
  return true;
}

const TreeItem *TreeItem::parent() const {
  return parentItem;
}

TreeItem *TreeItem::parent() {
  return parentItem;
}

bool TreeItem::removeChildren(unsigned int position, unsigned int count) {
  if (position + count > childItems.size())
    return false;
// TODO: strange: the following does not work - WHY?  
//  model->beginRemoveRows(model->createIndex(0,0,this), position, position+count-1);
  model->beginResetModel();
  for (unsigned int row = 0; row < count; ++row)
    childItems.release( childItems.begin() + position );
//  model->endRemoveRows();
  model->endResetModel();
  return true;
}

int TreeItem::childNumber() const {
  if (parentItem != NULL) {
    const ChildListType &pList = parentItem->childItems;
    for(ChildListType::size_type i = 0; i != pList.size(); ++i)
      if (&pList[i] == this) return i;
  }
  return 0;  
}

bool TreeItem::setData(int column, const QVariant &value) {
  return false;
}

Qt::ItemFlags TreeItem::flags(int column) const {
  return Qt::NoItemFlags;
}

void TreeItem::setActive(bool act) const {
  if (active==act) return;
  const_cast<TreeItem*>(this)->active = act;
  const_cast<TreeItem*>(this)->model->dataChanged(
    getIndex(0),
    getIndex(columnCount()-1));
}

QModelIndex TreeItem::getIndex(int column) const {
  return const_cast<TreeItem*>(this)->model->createIndex(childNumber(),column,parentItem);
}


void TreeItem::clearActiveDown(void) const {
  setActive(false);
  for(ChildListType::const_iterator i = childItems.begin(); i != childItems.end(); ++i)
    i->clearActiveDown();
}
