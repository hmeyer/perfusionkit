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
    ITKVTKTreeItem(TreeItem * parent=NULL, const typename TImage::Pointer itkI = typename TImage::Pointer()): VTKTreeItem(parent), itkImage(itkI) {}
    virtual typename TImage::Pointer getITKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0);
    virtual vtkImageData *getVTKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0) {
      if (connector.IsNull()) {
	connector = ConnectorType::New();
	connector->SetInput( getITKImage(progress, progressScale, progressBase) );
      }
      connector->Update();
      return connector->GetOutput();
    }
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
};




template< class TImage >
typename TImage::Pointer  ITKVTKTreeItem<TImage>::getITKImage(QProgressDialog *progress, int progressScale, int progressBase) {
  return itkImage;
}

#endif // ITKVTKTREEITEM_H
