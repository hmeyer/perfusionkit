#include "binaryimagetreeitem.h"


BinaryImageTreeItem::BinaryImageTreeItem(TreeItem * parent, BinaryImageType::Pointer itkImage, const QString &name)
  :BaseClass(parent, itkImage), name(name) {
}

TreeItem *BinaryImageTreeItem::clone(TreeItem *clonesParent) const {
  BinaryImageTreeItem *c = new BinaryImageTreeItem(clonesParent, itkImage, name );
  cloneChildren(c);
  return c;
}

int BinaryImageTreeItem::columnCount() const {
  return 1;
}
QVariant BinaryImageTreeItem::do_getData_DisplayRole(int c) const {
  if (c==0) return name;
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
