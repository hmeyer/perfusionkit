#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <itkMetaDataDictionary.h>
#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <QString>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/tracking.hpp>



#include "ctimagetreemodel.h"



BOOST_SERIALIZATION_SPLIT_FREE(itk::MetaDataDictionary)
BOOST_SERIALIZATION_SPLIT_FREE(QString)

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
  s = QString::fromStdString(string);
}

template<class Archive>
void save(Archive & ar, const QString &s, const unsigned int version)
{
  std::string string = s.toStdString();
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


template<class Archive, class PixelType, unsigned Dimension>
inline void load(Archive & ar, typename itk::SmartPointer< itk::Image<PixelType, Dimension> > &i, const unsigned int version)
{
  typedef typename itk::Image<PixelType, Dimension> ImageType;
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
  for(it.GoToBegin(); !it.IsAtEnd(); ++it) {
    ar & it.Value();
  }
}



template<class Archive, class PixelType, unsigned Dimension>
inline void save(Archive & ar, const typename itk::SmartPointer< itk::Image<PixelType, Dimension> > &i, const unsigned int version)
{
  typedef typename itk::Image<PixelType, Dimension> ImageType;
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
  for(it.GoToBegin(); !it.IsAtEnd(); ++it) {
    ar & it.Value();
  }
}


template<class Archive, class PixelType, unsigned Dimension>
inline void serialize(Archive & ar, typename itk::SmartPointer< itk::Image<PixelType, Dimension> > &i, const unsigned int version)
{
  boost::serialization::split_free(ar, i, version);
}


} // namespace serialization
}





#endif