#include "binaryimagetreeitem.h"


BinaryImageTreeItem::BinaryImageTreeItem(const TreeItem * parent):BaseClass( parent ) {}

TreeItem *BinaryImageTreeItem::clone(const TreeItem *clonesParent) const {
  if (clonesParent==NULL) clonesParent = parent();
  BinaryImageTreeItem *c = new BinaryImageTreeItem( clonesParent );
  c->name = name;
  cloneChildren(c);
  return c;
}

int BinaryImageTreeItem::columnCount() const {
  return 1;
}
QVariant BinaryImageTreeItem::data(int c, int role) const {
  if (c==0 && Qt::DisplayRole) return name;
  else return QVariant::Invalid;
}
bool BinaryImageTreeItem::setData(int c, const QVariant &value) {
  if (c==0 && static_cast<QMetaType::Type>(value.type()) == QMetaType::QString) {
    name = value.toString();
    return true;
  } else return false;
}
