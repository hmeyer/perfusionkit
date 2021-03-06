/*
    This file is part of perfusionkit.
    Copyright 2010 Henning Meyer

    perfusionkit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    perfusionkit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with perfusionkit.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include "ui_MainWindow.h"
#include "ctimagetreemodel.h"
#include "binaryimagetreeitem.h"
#include "ctimagetreeitem.h"
#include <vector>
#include <memory>
#include <boost/shared_ptr.hpp>

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
    void setFiles(const QStringList &names);
    

 public slots:
      void on_actionOpenDirectory_triggered();
      void on_actionOpenFile_triggered();
      void on_actionMemoryUsage_triggered();
      void on_actionExit_triggered();
      void on_actionAbout_triggered();
     
      void on_actionStereoDistance_triggered();
      void on_actionStereoEyeAngle_triggered();
      void on_actionStereoAnaglyph_triggered();
      void on_actionStereoInterlaced_triggered();
      void on_actionStereoOff_triggered();
      void on_actionLoadAllSeries_triggered();
      void on_actionCubicInterpolation_triggered();
      void on_actionSaveProject_triggered();
      void on_actionOpenProject_triggered();
      void on_treeView_doubleClicked(const QModelIndex &index);
      
      void onSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
      
      void treeViewContextMenu(const QPoint &pos);
      void removeSelectedImages();
      void createSegmentForSelectedImage();
      void changeColorForSelectedSegment();
      void setupSelectedWatershedSegment();
      void updateSelectedWatershedSegment();
      void on_buttonDraw_clicked();
      void on_buttonThreshold_clicked();
      void on_buttonRegionGrow_clicked();
      void on_buttonDilate_clicked();
      void on_buttonErode_clicked();
      void on_buttonWatershed_clicked();
      void on_buttonAnalyse_clicked();

 protected:
  void clearPendingAction();
 private:
  BinaryImageTreeItem *focusSegmentFromSelection(void);
  typedef std::auto_ptr<DicomSelectorDialog> DicomSelectorDialogPtr;
  
  void loadDicomData(DicomSelectorDialogPtr dicomSelector);
  void setImage(const CTImageTreeItem *imageItem);
  void segmentShow(const BinaryImageTreeItem *segItem );
  void segmentHide(const BinaryImageTreeItem *segItem );

  
  CTImageTreeModel imageModel;
  CTImageTreeItem::ConnectorHandle displayedCTImage;
  
  typedef std::set< BinaryImageTreeItem::ConnectorHandle > DisplayedSegmentContainer;
  DisplayedSegmentContainer displayedSegments;
  static const DicomTagList CTModelHeaderFields;
  int pendingAction;
  int maxMemoryUsageInMB;
};


#endif // MAINWINDOW_H
