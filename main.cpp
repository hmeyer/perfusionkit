#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkFlipImageFilter.h"
#include "itkImageToVTKImageFilter.h"

#include "mainwindow.h"
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
  MainWindow window;
  window.setImage( connector->GetOutput() );
  window.show();
/*
  
  VolumeProjectionWidget volImageWidget;
  volImageWidget.setImage( connector->GetOutput() );
  volImageWidget.show();
*/
  app.exec();
  
  return 0;
}