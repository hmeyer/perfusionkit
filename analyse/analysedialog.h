#ifndef ANALYSEDIALOG_H
#define ANALYSEDIALOG_H

#include <QDialog>
#include "ui_AnalyseDialog.h"
#include <set>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "segmentlistmodel.h"

class CTImageTreeItem;
class BinaryImageTreeItem;
class QwtPlotMarker;
class QwtPlotGrid;
class QwtPlotPicker;
class QwtPlotCurve;
class QRadioButton;
class QCheckBox;

class AnalyseDialog : public QDialog, private Ui_AnalyseDialog
{
  Q_OBJECT
  public:
  AnalyseDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  ~AnalyseDialog();
  void addImage(CTImageTreeItem *image);
  int exec(void);
  
  typedef boost::shared_ptr< QwtPlotCurve > CurvePtr;
  typedef std::map< const BinaryImageTreeItem *, CurvePtr > SegmentCurveMap;
  
  
 public slots:
      void on_sliderStart_valueChanged(int val);
      void on_sliderEnd_valueChanged(int val);
      void on_tableGamma_clicked(const QModelIndex & index);
      void on_tableGamma_activated(const QModelIndex & index);
      void on_listPatlak_clicked(const QModelIndex & index);
      void on_listPatlak_activated(const QModelIndex & index);
      void on_checkEnableGamma_toggled();
      void on_buttonArtery_selected(const SegmentInfo *segment);

  private:
    void recalculateGamma(SegmentInfo &seginfo);
  struct CTImageTimeCompareFunctor {
    typedef CTImageTreeItem * argT;
    bool operator()(const argT &x, const argT &y) const;
  };
  typedef std::set<CTImageTreeItem *, CTImageTimeCompareFunctor> ImageSet;
  typedef std::vector< double > DoubleVector;
  
  ImageSet images;
  DoubleVector times;
  QwtPlotMarker *markerStart, *markerEnd;
  QwtPlotMarker *markerPickerX, *markerPickerY;
  QwtPlotGrid *grid;
  QwtPlotPicker *picker;
  SegmentListModel segments;
};

#endif // ANALYSEDIALOG_H