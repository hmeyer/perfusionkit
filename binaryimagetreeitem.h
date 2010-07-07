#ifndef BINARYIMAGETREEITEM_H
#define BINARYIMAGETREEITEM_H

#include "itkvtktreeitem.h"
#include "imagedefinitions.h"
#include "QString"

#include <boost/function.hpp>

#include "serialization_helper.h"
#include <boost/serialization/array.hpp>
#include <boost/serialization/split_member.hpp>


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
    void regionGrow( float x, float y, float z, boost::function<void()> clearAction);
    const RGBType &getColor() { return color;}
    void thresholdParent(double lower, double upper);
    void binaryDilate(int iterations);
    void binaryErode(int iterations);
  private:
    void createRandomColor();
    QString name;
    RGBType color;

  private:
    friend class boost::serialization::access;
    BinaryImageTreeItem() {};
    template<class Archive>
    void load(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<BaseClass>(*this);
      ar & name;
      ar & color;
      BinaryImageType::Pointer binIm;
      ar & binIm;
      setITKImage( binIm );
    }
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const {
      ar & boost::serialization::base_object<BaseClass>(*this);
      ar & name;
      ar & color;
      BinaryImageType::Pointer binIm = peekITKImage();
      ar & binIm;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};




#endif // BINARYIMAGETREEITEM_H
