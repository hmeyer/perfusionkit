#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkFlipImageFilter.h"
#include "itkImageToVTKImageFilter.h"

#include "mainwindow.h"
#include <QApplication>

using namespace std;

int main( int argc, char **argv ) {

  QApplication app(argc, argv);
  MainWindow window;
  QStringList args = app.arguments();
  args.removeFirst();
  window.setFiles( args );
  window.show();
  
  app.exec();
  
  return 0;
}