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
    void drawSphere( float x, float y, float z );
  protected:
    typename ImageType::Pointer itkImage;
    typedef itk::ImageToVTKImageFilter< ImageType >   ConnectorType;
    typename ConnectorType::Pointer connector;
};



template< class TImage >
void ITKVTKTreeItem<TImage>::drawSphere( float x, float y, float z ) {
  typename ImageType::IndexType idx;
  typename ImageType::PointType point;
  point[0] = x;point[1] = y;point[2] = z;
  itkImage->TransformPhysicalPointToIndex(point, idx);
  std::cerr << __FUNCTION__ << "[" << x << " ," << y << " ," << z << "] idx:[" << idx[0] << " , " << idx[1] << " , " << idx[2] << "]" << std::endl;
  static bool i = true;
//  if (i) { i=false;itkImage->FillBuffer(0);}
  if (itkImage->GetBufferedRegion().IsInside(idx)) {
    std::cerr << "pixel before:" << int(itkImage->GetPixel(idx)) << std::endl;
    itkImage->SetPixel(idx, 255);
    std::cerr << "pixel after:" << int(itkImage->GetPixel(idx)) << std::endl;
  }
  getVTKImage()->Update();
}


template< class TImage >
typename TImage::Pointer  ITKVTKTreeItem<TImage>::getITKImage(QProgressDialog *progress, int progressScale, int progressBase) {
  return itkImage;
}

#endif // ITKVTKTREEITEM_H
