#ifndef WATERSHEDSEGMENTTREEITEM_H
#define WATERSHEDSEGMENTTREEITEM_H

#include <binaryimagetreeitem.h>
#include <ctimagetreeitem.h>
#include <QString>
#include <boost/serialization/access.hpp>

#include <itkBinaryThresholdImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkGradientMagnitudeRecursiveGaussianImageFilter.h>
#include "morphologicalwatershedfrommarkersfilter.h"

class WatershedSegmentTreeItem : public BinaryImageTreeItem
{
  public:
    typedef BinaryImageTreeItem BaseClass;
    typedef CTImageType GradMagImageType;
//    typedef RealImageType GradMagImageType;
    static WatershedSegmentTreeItem *Generate(CTImageTreeItem * parent);
    explicit WatershedSegmentTreeItem(CTImageTreeItem * parent, const QString &name);
    virtual TreeItem* clone(TreeItem* clonesParent = 0) const;
    bool setup(void);
    void update(void);
    virtual bool isA(const std::type_info &other) const { 
      if (typeid(WatershedSegmentTreeItem)==other) return true;
      if (typeid(BaseClass)==other) return true;
      if (typeid(BaseClass::BaseClass)==other) return true;
      if (typeid(BaseClass::BaseClass::BaseClass)==other) return true;
      return false;
    }
  protected:
    typedef itk::BinaryThresholdImageFilter< BinaryImageType, LabelImageType > ThresholdBin2LabFilterType;
    ThresholdBin2LabFilterType::Pointer thresholdInside;
    ThresholdBin2LabFilterType::Pointer thresholdOutside;
    typedef itk::AddImageFilter< LabelImageType, LabelImageType, LabelImageType > AddFilterType;
    AddFilterType::Pointer adder;
    typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<CTImageTreeItem::ImageType, GradMagImageType> GaussGradMagFilterType;
    GaussGradMagFilterType::Pointer gaussGradMag;
    typedef MorphologicalWatershedFromMarkersFilter<GradMagImageType, LabelImageType> WatershedFilterType;

    WatershedFilterType::Pointer watershed;
    typedef itk::BinaryThresholdImageFilter< LabelImageType, BinaryImageType > ThresholdLab2BinFilterType;
    ThresholdLab2BinFilterType::Pointer thresholdLabel;
    
    BinaryImageTreeItem *inside, *outside;
    
  private:
    void initFilters(void);
    WatershedSegmentTreeItem() {initFilters();};
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
    
    
};

#endif // WATERSHEDSEGMENTTREEITEM_H
