#ifndef ANALYSEDIALOG_H
#define ANALYSEDIALOG_H

#include <QDialog>
#include "ui_AnalyseDialog.h"
#include <set>
#include <vector>

class CTImageTreeItem;
class BinaryImageTreeItem;
class QwtPlotMarker;
class QwtPlotGrid;

class AnalyseDialog : public QDialog, private Ui_AnalyseDialog
{
  Q_OBJECT
  public:
  AnalyseDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  ~AnalyseDialog();
  void addImage(CTImageTreeItem *image);
  int exec(void);
  
  static double stringToSec(const std::string &timeString);
  
  
 public slots:
      void on_sliderStart_valueChanged(int val);
      void on_sliderEnd_valueChanged(int val);
  
  

  private:
  struct CTImageTimeCompareFunctor {
    typedef CTImageTreeItem * argT;
    bool operator()(const argT &x, const argT &y) const;
  };
  typedef std::set<CTImageTreeItem *, CTImageTimeCompareFunctor> ImageSet;
  typedef std::set<BinaryImageTreeItem *> SegmentSet;
  typedef std::vector< double > DoubleVector;
  ImageSet images;
  SegmentSet segments;
  DoubleVector times;
  QwtPlotMarker *markerStart, *markerEnd;
  QwtPlotGrid *grid;
};

#endif // ANALYSEDIALOG_H