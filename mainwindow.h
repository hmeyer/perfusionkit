#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "ui_MainWindow.h"
#include "ctimagetreemodel.h"
#include <vector>
#include <auto_ptr.h>

class vtkImageData;
class VolumeProjectionWidget;
class MultiPlanarReformatWidget;
class QSplitter;
class DicomSelectorDialog;




class MainWindow : public QMainWindow, private Ui_MainWindow
{
  Q_OBJECT
  
  public:
    MainWindow();
    ~MainWindow();
    void setImage(const vtkImageData *image);
    void setFiles(const QStringList &names);
    

 public slots:
      void on_actionOpenDirectory_triggered();
      void on_actionOpenFile_triggered();
      void on_actionExit_triggered();
      void on_actionAbout_triggered();
     
      void on_actionStereoDistance_triggered();
      void on_actionStereoEyeAngle_triggered();
      void on_actionStereoAnaglyph_triggered();
      void on_actionStereoInterlaced_triggered();
      void on_actionStereoOff_triggered();
      void on_actionLoadAllSeries_triggered();
      void treeViewSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
      void treeViewContextMenu(const QPoint &pos);

 private:
  typedef std::auto_ptr<DicomSelectorDialog> DicomSelectorDialogPtr;
  void loadDicomData(DicomSelectorDialogPtr dicomSelector);
  CTImageTreeModel imageModel;
  static const CTImageTreeItem::DicomTagList CTModelHeaderFields;
};


#endif // MAINWINDOW_H
