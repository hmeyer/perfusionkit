#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkFlipImageFilter.h"
#include "itkImageToVTKImageFilter.h"

#include "multiplanarreformatwidget.h"
#include "volumeprojectionwidget.h"
#include <QSplitter>
#include <QApplication>

using namespace std;

int main( int argc, char **argv ) {
  
  
  typedef itk::Image< signed short,3 >            ImageType;
  typedef itk::ImageFileReader< ImageType >         ReaderType;
  typedef itk::FlipImageFilter< ImageType > 		FlipFilterType;
  typedef itk::ImageToVTKImageFilter< ImageType >   ConnectorType;
  
  ImageType::Pointer itkimage;
  {
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[1] );
    reader->Update();
    FlipFilterType::Pointer flip = FlipFilterType::New();
    flip->SetInput( reader->GetOutput() );
    itkimage = flip->GetOutput();
    itkimage->Update();
  }
  
  ConnectorType::Pointer connector = ConnectorType::New();
  connector->SetInput( itkimage );

  QApplication app(argc, argv);

  
  QSplitter *splitter = new QSplitter;
     
  MultiPlanarReformatWidget volImageWidget;
  volImageWidget.setImage( connector->GetOutput() );
  volImageWidget.show();
  splitter->addWidget( &volImageWidget );

  VolumeProjectionWidget volImageWidget2;
  volImageWidget2.setImage( connector->GetOutput() );
  volImageWidget2.show();
  splitter->addWidget( &volImageWidget2 );

  splitter->show();

  app.exec();
  
  return 0;
}

