#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <QAbstractItemModel>
#include <set>
#include <itkMetaDataDictionary.h>
#include <itkImage.h>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <list>
#include <string>

const unsigned int Dimension = 3;


class vtkImageData;

class DicomImageListModel : public QAbstractItemModel {
  public:
    typedef boost::shared_ptr< DicomImageListModel > Pointer;
    typedef std::set< std::string > FileNameList;
    typedef std::pair< const std::string, const std::string > DicomTagType;
    typedef std::vector< DicomTagType > DicomTagListType;
    typedef signed short InputPixelType;
    typedef itk::Image< InputPixelType, Dimension> ImageType;
    class ListItem {
      public:
      ListItem( const std::string &seriesUID, const itk::MetaDataDictionary &d, const std::string &s)
	:seriesUID(seriesUID), dict(d) {fnList.insert(s);}
      const itk::MetaDataDictionary &getDictionary() const { return dict;}
      const FileNameList &getFileNames() const { return fnList; }
      bool imageDataLoaded() const { return itkImage.IsNotNull(); }
      int getNumberOfSlices() const;
      void appendFilename(const std::string &fn) { fnList.insert(fn);}
      ImageType::ConstPointer getITKImage(void) const;
      const vtkImageData *getVTKImage() const;
      void flip(void);
      const std::string seriesUID;
      private:
      itk::MetaDataDictionary dict;
      FileNameList fnList;
      ImageType::Pointer itkImage;
      struct ListItem_impl;
      boost::shared_ptr< ListItem_impl > impl;
    };
    void appendItem( const ListItem &item);
    void appendFilename( const itk::MetaDataDictionary &dict, const std::string &fname);
    void appendDicomImageList( const DicomImageListModel &other);
    const ListItem &getItem( const QModelIndex &index) const;
    int getNumberOfSlices( const QModelIndex &index) const;
    int rowCount( const QModelIndex &i = QModelIndex()) const;
    static const DicomTagListType dicomSelectorFields;

  private:
    typedef std::list< ListItem > SeriesListType;
    SeriesListType::iterator findByUID( const std::string &seriesUID);
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &i, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & idx ) const;
    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

    SeriesListType seriesList;
    static const std::string NumberOfFramesTag;
    static const std::string SeriesInstanceUIDTag;
};
  

#endif // IMAGEDATA_H