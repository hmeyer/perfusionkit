#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"

#include "volumeimagewidget.h"
#include <QSplitter>
#include <QApplication>

using namespace std;

int main( int argc, char **argv ) {
  
  
  typedef itk::Image< signed short,3 >            ImageType;
  typedef itk::ImageFileReader< ImageType >         ReaderType;
  typedef itk::ImageToVTKImageFilter< ImageType >   ConnectorType;
  ReaderType::Pointer reader = ReaderType::New();
  ConnectorType::Pointer connector = ConnectorType::New();
  reader->SetFileName( argv[1] );
  connector->SetInput( reader->GetOutput() );

  QApplication app(argc, argv);

  
  QSplitter *splitter = new QSplitter;
     
  VolumeImageWidget volImageWidget;
  volImageWidget.setImage( connector->GetOutput() );
  volImageWidget.show();
  splitter->addWidget( &volImageWidget );

  VolumeImageWidget volImageWidget2;
  volImageWidget2.setImage( connector->GetOutput() );
  volImageWidget2.show();
  splitter->addWidget( &volImageWidget2 );

  splitter->show();

  app.exec();
  
  return 0;
}

