#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>

class vtkImageData;
class VolumeProjectionWidget;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
  public:
    MainWindow();
    ~MainWindow();
    void setImage(vtkImageData *image);

 private slots:
     void open();
     void setDistance();  
     void setEyeAngle();
     void toggleStereo();
     void about();

 private:
     void createActions();
     void createMenus();

     QMenu *fileMenu;
     QMenu *viewMenu;
     QMenu *helpMenu;
     
     QAction *openAct;
     QAction *exitAct;
     QAction *setDistanceAct;
     QAction *setEyeAngleAct;
     QAction *toggleStereoAct;
     QAction *aboutAct;
     
     
     VolumeProjectionWidget *volImageWidget;
};


#endif // MAINWINDOW_H
