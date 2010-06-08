#include "binaryimagetreeitem.h"


BinaryImageTreeItem::BinaryImageTreeItem(const TreeItem * parent, BinaryImageType::Pointer itkImage, const QString &name)
  :BaseClass(parent, itkImage), name(name) {
}

TreeItem *BinaryImageTreeItem::clone(const TreeItem *clonesParent) const {
  if (clonesParent==NULL) clonesParent = parent();
  BinaryImageTreeItem *c = new BinaryImageTreeItem( clonesParent, itkImage, name );
  cloneChildren(c);
  return c;
}

int BinaryImageTreeItem::columnCount() const {
  return 1;
}
QVariant BinaryImageTreeItem::data(int c, int role) const {
  if (c==0 && role==Qt::DisplayRole) return name;
  else return QVariant::Invalid;
}

Qt::ItemFlags BinaryImageTreeItem::flags(int column) const {
    if (column != 0) return Qt::NoItemFlags;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;  
}


bool BinaryImageTreeItem::setData(int c, const QVariant &value) {
  if (c==0 && static_cast<QMetaType::Type>(value.type()) == QMetaType::QString) {
    name = value.toString();
    return true;
  } 
  return false;
}
