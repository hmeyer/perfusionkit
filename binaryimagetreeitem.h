#ifndef BINARYIMAGETREEITEM_H
#define BINARYIMAGETREEITEM_H

#include "itkvtktreeitem.h"
#include "imagedefinitions.h"
#include "QString"




class BinaryImageTreeItem : public ITKVTKTreeItem< BinaryImageType > {
  public:
    typedef ITKVTKTreeItem< BinaryImageType > BaseClass;
    BinaryImageTreeItem(TreeItem * parent, BinaryImageType::Pointer itkImage, const QString &name);
    virtual TreeItem *clone(TreeItem *clonesParent=NULL) const;
    virtual int columnCount() const;
    virtual QVariant do_getData_DisplayRole(int c) const;
    virtual QVariant do_getData_BackgroundRole(int column) const;
    virtual Qt::ItemFlags flags(int column) const;
    virtual bool setData(int c, const QVariant &value);
    const QString &getName() const { return name; }
    void setName(const QString &_name) { name = _name; }
    const unsigned char *getColor() { return color;}
  private:
    void createRandomColor();
    QString name;
    unsigned char color[3];
};

#endif // BINARYIMAGETREEITEM_H
