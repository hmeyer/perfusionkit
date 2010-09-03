#ifndef BINARYIMAGETREEITEM_H
#define BINARYIMAGETREEITEM_H

#include "itkvtktreeitem.h"
#include "imagedefinitions.h"
#include <QString>
#include <QColor>
#include <boost/function.hpp>
#include <boost/serialization/access.hpp>

class BinaryImageTreeItem : public ITKVTKTreeItem< BinaryImageType > {
  public:
    typedef ITKVTKTreeItem< BinaryImageType > BaseClass;
    BinaryImageTreeItem(TreeItem * parent, ImageType::Pointer itkImage, const QString &name);
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
    const QColor &getColor() const { return color;}
    void setColor(const QColor &newColor) { color = newColor; }
    void thresholdParent(double lower, double upper);
    bool watershedParent(const BinaryImageTreeItem *includedSegment, const BinaryImageTreeItem *excludedSegment);
    void binaryDilate(int iterations);
    void binaryErode(int iterations);
    double getVolumeInML(void) const;
    virtual bool isA(const std::type_info &other) const { 
      if (typeid(BinaryImageTreeItem)==other) return true;
      if (typeid(BaseClass)==other) return true;
      if (typeid(BaseClass::BaseClass)==other) return true;
      return false;
    }
    
  private:
    void createRandomColor();
    QString name;
    QColor color;

  private:
    ConnectorHandle imageKeeper;
    BinaryImageTreeItem() {};
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};




#endif // BINARYIMAGETREEITEM_H
