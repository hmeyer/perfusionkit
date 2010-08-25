#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <itkMetaDataDictionary.h>
#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <QString>
#include <boost/foreach.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <bitset>
#include "imagedefinitions.h"
#include "ctimagetreemodel.h"
#include "ctimagetreeitem.h"
#include <binaryimagetreeitem.h>



BOOST_SERIALIZATION_SPLIT_FREE(itk::MetaDataDictionary)
BOOST_SERIALIZATION_SPLIT_FREE(QString)
BOOST_SERIALIZATION_SPLIT_FREE(CTImageTreeItem::SegmentationValueMap)


namespace boost {
namespace serialization {

template<class Archive>
void load(Archive & ar, itk::MetaDataDictionary &d, const unsigned int version)
{
  std::vector< std::string >::size_type keyNum;
  ar & keyNum;
  while(keyNum) {
    std::string key, value;
    ar & key;
    ar & value;
    itk::EncapsulateMetaData(d, key, value);
    --keyNum;
  }  
}

template<class Archive>
void save(Archive & ar, const itk::MetaDataDictionary &d, const unsigned int version)
{
  typedef std::vector< std::string > KeyContainer;
  KeyContainer keys = d.GetKeys();
  KeyContainer::size_type keyNum = keys.size();
  ar & keyNum;
  for(KeyContainer::const_iterator it = keys.begin(); it != keys.end(); ++it) {
    std::string val;
    itk::ExposeMetaData(d, *it, val);
    ar & *it;
    ar & val;
  }
}


template<class Archive>
void load(Archive & ar, QString &s, const unsigned int version)
{
  std::string string;
  ar & string;
  s = QString::fromAscii(string.c_str());
}

template<class Archive>
void save(Archive & ar, const QString &s, const unsigned int version)
{
  std::string string( s.toAscii().data() );
  ar & string;
}


template<class Archive, class U>
void load(Archive & ar, boost::ptr_vector<U> &v, const unsigned int version)
{
  typename boost::ptr_vector<U>::size_type size;
  ar & size;
  while(size) {
    TreeItem *child;
    ar & child;
    v.push_back(child);
    --size;
  }
}

template<class Archive, class U>
void save(Archive & ar, const boost::ptr_vector<U> &v, const unsigned int version)
{
  typename boost::ptr_vector<U>::size_type size = v.size();
  ar & size;
  for(typename boost::ptr_vector<U>::const_iterator it = v.begin(); it != v.end(); ++it) {
    const U *obj = &(*it);
    ar & obj;
  }
}

template<class Archive, class U>
inline void serialize(Archive & ar, boost::ptr_vector<U> &v, const unsigned int version)
{
  boost::serialization::split_free(ar, v, version);
}


template<class Archive, unsigned Dimension>
inline void load(Archive & ar, typename itk::SmartPointer< itk::Image<BinaryPixelType, Dimension> > &i, const unsigned int version)
{
  typedef typename itk::Image<BinaryPixelType, Dimension> ImageType;
  i = ImageType::New();
  typename ImageType::SpacingType spacing;
  typename ImageType::SizeType size;
  typename ImageType::IndexType index;
  typename ImageType::PointType origin;
  for(unsigned d = 0; d < Dimension; d++) {
    ar & size[d];
    ar & index[d];
    ar & spacing[d];
    ar & origin[d];
  }
  typename ImageType::RegionType region;
  region.SetIndex( index );
  region.SetSize( size );
  i->SetRegions( region );
  i->SetSpacing( spacing );
  i->SetOrigin( origin );
  i->Allocate();
  itk::ImageRegionIterator<ImageType> it(i,region);
  
  unsigned long long ulongVal = 0;
  typedef std::bitset< 8 * sizeof( ulongVal ) > UlongBitSet;
  UlongBitSet bitValues;
  size_t sizeCount = bitValues.size();
  for(it.GoToBegin(); !it.IsAtEnd(); ++it) {
    if (sizeCount == bitValues.size()) {
      ar & ulongVal;
	  UlongBitSet tb( ulongVal );
      std::swap( bitValues, tb );
      sizeCount = 0;
    }
    it.Value() = bitValues[sizeCount]? BinaryPixelOn : BinaryPixelOff;
    sizeCount++;
  }
}



template<class Archive, unsigned Dimension>
inline void save(Archive & ar, const typename itk::SmartPointer< itk::Image<BinaryPixelType, Dimension> > &i, const unsigned int version)
{
  typedef typename itk::Image<BinaryPixelType, Dimension> ImageType;
  typename ImageType::RegionType region = i->GetBufferedRegion();
  typename ImageType::SpacingType spacing = i->GetSpacing();
  typename ImageType::SizeType size = region.GetSize();
  typename ImageType::IndexType index = region.GetIndex();
  typename ImageType::PointType origin = i->GetOrigin();
  for(unsigned d = 0; d < Dimension; d++) {
    ar & size[d];
    ar & index[d];
    ar & spacing[d];
    ar & origin[d];
  }
  itk::ImageRegionConstIterator<ImageType> it(i,region);
  unsigned long ulongVal;
  std::bitset< sizeof( ulongVal ) > bitValues;
  size_t sizeCount = 0;
  for(it.GoToBegin(); !it.IsAtEnd(); ++it) {
    bitValues[sizeCount] = (it.Value() == BinaryPixelOn);
    sizeCount++;
    if (sizeCount == bitValues.size()) {
      ulongVal = bitValues.to_ulong();
      ar & ulongVal;
      sizeCount = 0;
    }
  }
  if (sizeCount != 0) {
      ulongVal = bitValues.to_ulong();
      ar & ulongVal;
  }
}


template<class Archive, unsigned Dimension>
inline void serialize(Archive & ar, typename itk::SmartPointer< itk::Image<BinaryPixelType, Dimension> > &i, const unsigned int version)
{
  boost::serialization::split_free(ar, i, version);
}


template<class Archive>
inline void load(Archive & ar, CTImageTreeItem::SegmentationValueMap &svm, const unsigned int version)
{
  size_t s;
  ar & s;
  while(s) {
    SegmentationValues sv;
    ar & sv;
    svm.insert(CTImageTreeItem::SegmentationValueMap::value_type(sv.segment, sv));
    --s;
  }
}

template<class Archive>
inline void save(Archive & ar, const CTImageTreeItem::SegmentationValueMap &svm, const unsigned int version)
{
  size_t s = svm.size();
  ar & s;
  BOOST_FOREACH( const CTImageTreeItem::SegmentationValueMap::value_type &svm_value, svm ) {
    const SegmentationValues &sv = svm_value.second;
    ar & sv;
  }
}

template<class Archive>
void serialize(Archive & ar, QColor &color, const unsigned int version) {
  unsigned char t;
  t = color.red(); ar & t; color.setRed(t);
  t = color.green(); ar & t; color.setGreen(t);
  t = color.blue(); ar & t; color.setBlue(t);
}



} // namespace serialization
}

template<class Archive>
void CTImageTreeModel::serialize(Archive & ar, const unsigned int version) {
  beginResetModel(); 
  ar & HeaderFields;
  ar & rootItem;
  endResetModel(); 
}

template<class Archive>
void TreeItem::serialize(Archive & ar, const unsigned int version) {
  ar & model;
  ar & parentItem;
  ar & childItems;
}
    
template<class TImage>
template<class Archive>
void ITKVTKTreeItem<TImage>::serialize(Archive & ar, const unsigned int version) {
  ar & boost::serialization::base_object<TreeItem>(*this);
}

template<class Archive>
void BinaryImageTreeItem::serialize(Archive & ar, const unsigned int version) {
  ar & name;
  ar & color;
  ImageType::Pointer binIm = peekITKImage();
  ar & binIm;
  ar & boost::serialization::base_object<BaseClass>(*this);
  setITKImage( binIm );
  imageKeeper = getVTKConnector();
}

template<class Archive>
void CTImageTreeItem::serialize(Archive & ar, const unsigned int version) {
  ar & boost::serialization::base_object<BaseClass>(*this);
  ar & itemUID;
  ar & fnList;
  ar & HeaderFields;
  ar & dict;
  ar & segmentationValueCache;
}

template<class Archive>
void SegmentationValues::serialize(Archive & ar, const unsigned int version) {
  ITKVTKTreeItem<BinaryImageType> *nonconstseg = const_cast<ITKVTKTreeItem<BinaryImageType> *>(segment);
  ar & nonconstseg;
  segment = nonconstseg;
  ar & mean; ar & stddev; ar & min; ar & max; ar & sampleCount; ar & accuracy;
  bool matchingMtime = mtime == segment->getITKMTime(); 
  ar & matchingMtime;
  if (matchingMtime)
    mtime = segment->getITKMTime();
  else mtime = 0; 
}

template<class Archive>
void DicomTagType::serialize(Archive & ar, const unsigned int version) {
  ar & name;
  ar & tag;
}



#endif