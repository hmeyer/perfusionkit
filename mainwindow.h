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
class BinaryImageTreeItem;




class MainWindow : public QMainWindow, private Ui_MainWindow
{
  Q_OBJECT
  
  public:
    MainWindow();
    ~MainWindow();
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
      void on_actionCubicInterpolation_triggered();
      void on_treeView_doubleClicked(const QModelIndex &index);
      void treeViewContextMenu(const QPoint &pos);
      void removeCTImage(int number);
      void on_buttonDraw_clicked();
      void on_buttonThreshold_clicked();
      void on_buttonDilate_clicked();
      void on_buttonErode_clicked();

 private:
  BinaryImageTreeItem *focusSegmentFromSelection(void);
  typedef std::auto_ptr<DicomSelectorDialog> DicomSelectorDialogPtr;
  void loadDicomData(DicomSelectorDialogPtr dicomSelector);
  void setImage(VTKTreeItem *imageItem);
  void segmentShow( BinaryImageTreeItem *segItem );
  void segmentHide( BinaryImageTreeItem *segItem );

  
  CTImageTreeModel imageModel;
  VTKTreeItem *selectedCTImage;
  
  typedef std::set< BinaryImageTreeItem* > DisplayedSegmentContainer;
  DisplayedSegmentContainer displayedSegments;
  static const CTImageTreeItem::DicomTagList CTModelHeaderFields;
};


#endif // MAINWINDOW_H
