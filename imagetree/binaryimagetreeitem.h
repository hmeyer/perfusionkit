#ifndef BINARYIMAGETREEITEM_H
#define BINARYIMAGETREEITEM_H

#include "itkvtktreeitem.h"
#include "imagedefinitions.h"
#include "QString"
#include <boost/function.hpp>
#include <boost/serialization/access.hpp>

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
    void drawSphere( float radius, float x, float y, float z, bool erase );
    void regionGrow( float x, float y, float z, boost::function<void()> postGrowingAction);
    const RGBType &getColor() const { return color;}
    void thresholdParent(double lower, double upper);
    void binaryDilate(int iterations);
    void binaryErode(int iterations);
  private:
    void createRandomColor();
    QString name;
    RGBType color;

  private:
    BinaryImageTreeItem() {};
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};




#endif // BINARYIMAGETREEITEM_H
