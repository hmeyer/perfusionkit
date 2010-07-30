#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkFlipImageFilter.h"
#include "itkImageToVTKImageFilter.h"

#include "mainwindow.h"
#include <QApplication>


using namespace std;



class applicationClass : public QApplication {
  public: 
   applicationClass(int _argc, char **_argv): QApplication(_argc, _argv)
    { 
    }
  bool notify(QObject *receiver, QEvent *e)
    {
      cout << "Notify()! receiver:" << receiver->objectName().toStdString() << endl;
      return QApplication::notify(receiver, e);
    } 
 };


int main( int argc, char **argv ) {
  QApplication app(argc, argv);
  
  QCoreApplication::setOrganizationName("UHN");
  QCoreApplication::setOrganizationDomain("uhn.on.ca");
  QCoreApplication::setApplicationName("perfusionkit");  
  
//  applicationClass app(argc, argv);

  MainWindow window;
  QStringList args = app.arguments();
  args.removeFirst();
  window.setFiles( args );
  window.show();
  
  app.exec();
  return 0;
}