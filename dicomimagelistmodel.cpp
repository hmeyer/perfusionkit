#include "dicomimagelistmodel.h"
#include "itkFlipImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <itkMetaDataObject.h>
#include "itkbasics.h"


struct DicomImageListModel::ListItem::ListItem_impl {
  typedef itk::ImageToVTKImageFilter< ImageType >   ConnectorType;
  ConnectorType::Pointer connector;
  ListItem_impl(): connector(ConnectorType::New()) {}
};

void DicomImageListModel::ListItem::flip(void) {
  typedef itk::FlipImageFilter< ImageType >  FlipFilterType;
  FlipFilterType::Pointer flip = FlipFilterType::New();
  flip->SetInput( itkImage );
  flip->Update();
  itkImage = flip->GetOutput();
}

const vtkImageData *DicomImageListModel::ListItem::getVTKImage(void) const {
  if (!impl) {
    ListItem *nonconst_this = const_cast< ListItem *>( this );
    nonconst_this->impl.reset( new ListItem_impl );
    nonconst_this->impl->connector->SetInput( getITKImage() );
    nonconst_this->impl->connector->Update();
  }
  return impl->connector->GetOutput();
}

DicomImageListModel::ImageType::ConstPointer DicomImageListModel::ListItem::getITKImage() const {
  if (itkImage.IsNull()) {
    itkBasic::ReaderType::Pointer imageReader = itkBasic::ReaderType::New();
    itkBasic::FileNamesContainer fc;
    fc.assign( fnList.begin(), fnList.end() );
    ListItem *nonconst_this = const_cast< ListItem *>( this );
    nonconst_this->itkImage = itkBasic::getDicomSerie( fc, imageReader );
    nonconst_this->dict = *((*imageReader->GetMetaDataDictionaryArray())[0]);
  }
  DicomImageListModel::ImageType::ConstPointer result;
  result = itkImage;
  return result;
}

int DicomImageListModel::ListItem::getNumberOfSlices() const {
  if (imageDataLoaded()) {
    DicomImageListModel::ImageType::RegionType imageRegion = itkImage->GetLargestPossibleRegion();
    return static_cast<uint>(imageRegion.GetSize(2));
  }
  int num = fnList.size();
  if (num > 1) return num;
  else {
    std::string t;
    itk::ExposeMetaData( dict, NumberOfFramesTag, t );
    std::istringstream buffer(t);
    buffer >> num;
    return num;
  }
}

const std::string DicomImageListModel::NumberOfFramesTag = "0028|0008";
const std::string DicomImageListModel::SeriesInstanceUIDTag = "0020|000e";
const DicomImageListModel::DicomTagListType DicomImageListModel::dicomSelectorFields = boost::assign::list_of
  (DicomTagType("Patient Name", "0010|0010"))
  (DicomTagType("Date of Birth","0010|0030"))
  (DicomTagType("Series Description","0008|103e"))
  (DicomTagType("#Slices",NumberOfFramesTag))
  (DicomTagType("SeriesDate","0008|0021"))
  (DicomTagType("SeriesTime","0008|0031"));

int DicomImageListModel::rowCount( const QModelIndex &i) const {
  return seriesList.size();
}
QVariant DicomImageListModel::data(const QModelIndex &i, int role) const {
  if (role == Qt::DisplayRole) {
    if (dicomSelectorFields[ i.column()].second == NumberOfFramesTag) return getNumberOfSlices(i);
    std::string val;
    itk::ExposeMetaData( getItem(i).getDictionary(), dicomSelectorFields[ i.column()].second, val );
    return QString( val.c_str() );
  } else {
    return QVariant::Invalid;
  }
}
QVariant DicomImageListModel::headerData( int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole) {
    return QString(dicomSelectorFields.at(section).first.c_str());
  } else {
    return QVariant::Invalid;
  }
}
int DicomImageListModel::columnCount(const QModelIndex &i) const {
  return dicomSelectorFields.size();
}
Qt::ItemFlags DicomImageListModel::flags ( const QModelIndex & index ) const {
  if (index.isValid())
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  else return Qt::NoItemFlags;
}
QModelIndex DicomImageListModel::index( int row, int column, const QModelIndex & parent) const {
  return createIndex( row, column );
}
QModelIndex DicomImageListModel::parent( const QModelIndex & idx ) const {
  return QModelIndex();
}
bool DicomImageListModel::hasChildren ( const QModelIndex & parent) const {
  if (!parent.isValid()) return true;
  return false;
}
DicomImageListModel::SeriesListType::iterator DicomImageListModel::findByUID( const std::string &seriesUID) {
  using boost::bind;
  using boost::ref;
  return std::find_if(seriesList.begin(), seriesList.end(), bind( &ListItem::seriesUID, _1 ) == ref( seriesUID ));
}
void DicomImageListModel::appendItem( const ListItem &item) {
  if (item.seriesUID.empty()) return;
  SeriesListType::iterator pos = findByUID( item.seriesUID );
  if (pos == seriesList.end()) {
    int endpos = rowCount();
    beginInsertRows(QModelIndex(),endpos,endpos);
    seriesList.push_back( item );
    endInsertRows();
  }
}
void DicomImageListModel::appendFilename( const itk::MetaDataDictionary &dict, const std::string &fname) {
  std::string sUID;
  itk::ExposeMetaData( dict, SeriesInstanceUIDTag, sUID );
  if (sUID.empty()) return;
  SeriesListType::iterator pos = findByUID( sUID );
  if (pos != seriesList.end()) pos->appendFilename( fname );
  else {
    int endpos = rowCount();
    beginInsertRows(QModelIndex(),endpos,endpos);
    seriesList.push_back( ListItem( sUID, dict, fname ) );
    endInsertRows();
  }
}
void DicomImageListModel::appendDicomImageList(const DicomImageListModel &other) {
  typedef std::list< const ListItem* > ListItemPtrList;
  ListItemPtrList insertList;
  std::string sUID;
  for(SeriesListType::const_iterator it = other.seriesList.begin(); it != other.seriesList.end(); it++) {
    itk::ExposeMetaData( it->getDictionary(), SeriesInstanceUIDTag, sUID );
    SeriesListType::iterator pos = findByUID( sUID );
    if (pos == seriesList.end()) insertList.push_back(&*it);
  }
  int endpos = rowCount();
  if (insertList.size()) {
    beginInsertRows(QModelIndex(),endpos,endpos + insertList.size()-1);
    for(ListItemPtrList::const_iterator it = insertList.begin(); it != insertList.end(); it++) {
      seriesList.push_back( **it );
    }
    endInsertRows();
  }
}
const DicomImageListModel::ListItem &DicomImageListModel::getItem( const QModelIndex &index) const {
  int r = index.row();
  SeriesListType::const_iterator it = seriesList.begin();
  while( r-- ) it++;
  return *it; 
}

int DicomImageListModel::getNumberOfSlices( const QModelIndex &index ) const {
  return getItem(index).getNumberOfSlices();
}
