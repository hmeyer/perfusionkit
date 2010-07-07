#ifndef ITKVTKTREEITEM_H
#define ITKVTKTREEITEM_H

#include <vtktreeitem.h>
#include <itkImageToVTKImageFilter.h>
#include <itkCommand.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <boost/scoped_ptr.hpp>
#include <QProgressDialog>
#include <QObject>
#include <set>
#include <string>
#include <boost/bind.hpp>

template< class TImage >
class ITKVTKTreeItem : public VTKTreeItem {
  public:
    typedef ITKVTKTreeItem< TImage > MyType;
    typedef TImage ImageType;
    ITKVTKTreeItem(TreeItem * parent, const typename TImage::Pointer itkI = typename TImage::Pointer()): VTKTreeItem(parent), itkImage(itkI), connector(0) {}
    virtual typename TImage::Pointer getITKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0);
    virtual vtkImageData *getVTKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0) {
      if (connector.IsNull()) {
	connector = ConnectorType::New();
	connector->SetInput( getITKImage(progress, progressScale, progressBase) );
      }
      connector->Update();
      return connector->GetOutput();
    }
    ~ITKVTKTreeItem() { if (connector.IsNotNull()) connector->Delete(); connector=0; }
  protected:
    inline typename ImageType::Pointer peekITKImage(void) const { 
      return const_cast< MyType* >(this)->itkImage; 
    }
    inline void setITKImage(typename ImageType::Pointer image) { 
      itkImage = image; 
      if (connector.IsNotNull()) connector->SetInput(image);
    }
  private:
    typename ImageType::Pointer itkImage;
    typedef itk::ImageToVTKImageFilter< ImageType >   ConnectorType;
    typename ConnectorType::Pointer connector;
    
  protected:
    ITKVTKTreeItem() {}

  private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<VTKTreeItem>(*this);
    }
};




template< class TImage >
typename TImage::Pointer  ITKVTKTreeItem<TImage>::getITKImage(QProgressDialog *progress, int progressScale, int progressBase) {
  return itkImage;
}

#endif // ITKVTKTREEITEM_H
