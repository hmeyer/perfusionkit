#ifndef BINARYIMAGETREEITEM_H
#define BINARYIMAGETREEITEM_H

#include "itkvtktreeitem.h"
#include "imagedefinitions.h"
#include "QString"




class BinaryImageTreeItem : public ITKVTKTreeItem< BinaryImageType > {
  public:
    typedef ITKVTKTreeItem< BinaryImageType > BaseClass;
    BinaryImageTreeItem(const TreeItem * parent, BinaryImageType::Pointer itkImage, const QString &name);
    virtual TreeItem *clone(const TreeItem *clonesParent=NULL) const;
    virtual int columnCount() const;
    virtual QVariant data(int c, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(int column) const;
    virtual bool setData(int c, const QVariant &value);
    const QString &getName() const { return name; }
    void setName(const QString &_name) { name = _name; }
  private:
    QString name;
};

#endif // BINARYIMAGETREEITEM_H
