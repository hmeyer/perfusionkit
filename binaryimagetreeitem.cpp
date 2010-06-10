#include "binaryimagetreeitem.h"
#include <boost/random.hpp>


BinaryImageTreeItem::BinaryImageTreeItem(TreeItem * parent, BinaryImageType::Pointer itkImage, const QString &name)
  :BaseClass(parent, itkImage), name(name) {
    createRandomColor();
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

QVariant BinaryImageTreeItem::do_getData_BackgroundRole(int column) const {
  QColor qcolor( color[0], color[1], color[2]);
  return QBrush( qcolor );
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

void BinaryImageTreeItem::createRandomColor() {
  static boost::mt19937 rng;
  static boost::uniform_int<> rainbow(0,256*6);
  static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rainbowcolor(rng, rainbow);
  int hue = rainbowcolor();     
  if (hue<256) {
    int rise = hue;
      color[0] = 255;
      color[1] = rise;
      color[2] = 0;
  } else if (hue<512) {
    int fall = 511-hue;
      color[0] = fall;
      color[1] = 255;
      color[2] = 0;
  } else if (hue<768) {
    int rise = hue-512;
      color[0] = 0;
      color[1] = 255;
      color[2] = rise;
  } else if (hue<1024) {
    int fall = 1023-hue;
      color[0] = 0;
      color[1] = fall;
      color[2] = 255;
  } else if (hue<1280) {
    int rise = hue-1024;
      color[0] = rise;
      color[1] = 0;
      color[2] = 255;
  } else { //if (color<1536) 
    int fall = 1535-hue;
      color[0] = 255;
      color[1] = 0;
      color[2] = fall;
  }
}
